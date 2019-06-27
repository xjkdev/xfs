#include <include/globals.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <include/xfs/general.h>
#include <stdlib.h>
#define min(a, b) ((a) < (b) ? (a) : (b))

int _indirect_zone1_read(struct inode_struct *inode, diskptr_t indir_block,
                         char *buf, xsize_t nbyte, xsize_t offset);
int _direct_zone_read(diskptr_t blocks[], char *buf, xsize_t nbyte,
                      xsize_t offset) {
  xsize_t direct_max_size = loaded_xfs.dz_per_inode * loaded_xfs.block_size;
  int block_index = offset / loaded_xfs.block_size;
  int res, read_bytes = 0;
  offset = offset % loaded_xfs.block_size;
  // 前半部分
  res = disk_read(blocks[block_index] + offset, buf,
                  min(loaded_xfs.block_size - offset, nbyte));
  if (res == -1)
    return -1;
  nbyte -= res;
  read_bytes += res;
  block_index++;

  while (nbyte >= loaded_xfs.block_size) { // 读整块的部分
    res = disk_read(blocks[block_index], buf, loaded_xfs.block_size);
    if (res == -1)
      return -1;
    nbyte -= res;
    read_bytes += res;
    block_index++;
  }
  if (nbyte > 0) { // 读剩余部分
    res = disk_read(blocks[block_index], buf, nbyte);
    if (res == -1)
      return -1;
    nbyte -= res;
    read_bytes += res;
  }
  return read_bytes;
}

int _indirect_zone2_read(struct inode_struct *inode, char *buf, xsize_t nbyte,
                         xsize_t offset) {
  xsize_t indir_block1_max = loaded_xfs.idz_per_inode * loaded_xfs.block_size;
  diskptr_t *blocks = malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t));
  int res, read_bytes = 0;
  int i = offset / loaded_xfs.block_size;
  offset = offset % loaded_xfs.block_size;
  if (disk_read(inode->indir_block2, blocks, loaded_xfs.block_size) ==
      -1) { // 加载二级间接块的第一层
    free(blocks);
    return -1;
  }
  // 把二级间接块第二层当做一级间接块处理
  res = _indirect_zone1_read(
      NULL, blocks[i], buf, min(loaded_xfs.block_size - offset, nbyte), offset);
  if (res == -1) {
    free(blocks);
    return -1;
  }
  read_bytes += res;
  nbyte -= res;
  buf += res;
  i++;
  // 读取整层的
  while (nbyte >= indir_block1_max) {
    res = _indirect_zone1_read(NULL, blocks[i], buf, indir_block1_max, 0);
    if (res == -1) {
      free(blocks);
      return -1;
    }
    read_bytes += res;
    buf += res;
    nbyte -= res;
    i++;
  }
  if (nbyte > 0) {
    res = _indirect_zone1_read(NULL, blocks[i], buf, nbyte, 0);
    if (res == -1) {
      free(blocks);
      return -1;
    }
    read_bytes += res;
  }
  free(blocks);
  return read_bytes;
}

int _indirect_zone1_read(struct inode_struct *inode, diskptr_t indir_block,
                         char *buf, xsize_t nbyte, xsize_t offset) {
  xsize_t indir_block1_max = loaded_xfs.idz_per_inode * loaded_xfs.block_size;
  diskptr_t *blocks = malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t));
  if (disk_read(indir_block, blocks, loaded_xfs.block_size) ==
      -1) // 加载间接块表
    return -1;
  if (offset + nbyte < indir_block1_max) { // 情况1: 全部在一级间接块
    // TODO: 优化，不需要读这么多
    int res = _direct_zone_read(blocks, buf, nbyte, offset);
    free(blocks);
    return res;
  } else if (!inode) { // 如果inode没有给定，不用执行之后的
    free(blocks);
    return -1;
  } else if (offset < indir_block1_max) {
    // 情况2: 部分在一级间接块，部分在二级间接块
    int res2, res1 = _direct_zone_read(blocks, buf, indir_block1_max - offset,
                                       offset);
    nbyte -= res1;
    if (res1 == -1 || !inode) {
      free(blocks);
      return res1;
    }
    res2 = _indirect_zone2_read(inode, buf + res1, nbyte, 0);
    if (res2 == -1) {
      free(blocks);
      return res2;
    }
    free(blocks);
    return res1 + res2;
  } else { // 情况3: 全部在二级间接块
    int res =
        _indirect_zone2_read(inode, buf, nbyte, offset - indir_block1_max);
    free(blocks);
    return res;
  }
}

int _read_inode(struct inode_struct *inode, char *buf, xsize_t nbyte,
                xsize_t offset) {
  xsize_t direct_max_size;
  if (nbyte + offset > inode->file_size) {
    nbyte = inode->file_size - offset;
  }
  direct_max_size = loaded_xfs.dz_per_inode * loaded_xfs.block_size;
  if (offset + nbyte < direct_max_size) { // 情况1: 全部在直接块区
    return _direct_zone_read(inode->block, buf, nbyte, offset);
  } else if (offset < direct_max_size) { // 情况2: 部分在直接块区，部分间接块区
    int res1, res2;
    res1 =
        _direct_zone_read(inode->block, buf, direct_max_size - offset, offset);
    nbyte -= res1;
    if (res1 == -1) {
      return -1;
    }
    res2 = _indirect_zone1_read(inode, inode->indir_block, buf, nbyte, 0);
    if (res2 == -1) {
      return -1; // 这里是返回之前部分还是不返回
    }
    return res1 + res2;
  } else { // 情况3: 全部在间接块区
    return _indirect_zone1_read(inode, inode->indir_block, buf, nbyte,
                                offset - direct_max_size);
  }
}

xsize_t xfs_read(int fildes, char *buf, xsize_t nbyte) {
  struct fd_struct *fd = fd_table_search(fildes);
  if (fd == NULL)
    return -1;
  if (!(fd->mod & O_ACCMODE == O_RDONLY || fd->mod & O_ACCMODE == O_RDWR)) {
    return -1;
  }
  return _read_inode(fd->inode, buf, nbyte, fd->offset);
}
