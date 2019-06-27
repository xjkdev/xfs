#include <include/globals.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <include/xfs/general.h>
#include <stdlib.h>
#define min(a, b) ((a) < (b) ? (a) : (b))

int _direct_zone_write(diskptr_t blocks[], char *buf, xsize_t nbyte,
                       xsize_t offset) {
  xsize_t direct_max_size = loaded_xfs.dz_per_inode * loaded_xfs.block_size;
  int block_index = offset / loaded_xfs.block_size;
  int res, read_bytes = 0;
  offset = offset % loaded_xfs.block_size;
  // 前半部分
  if (blocks[block_index] == DA_NULL) {
    blocks[block_index] = block_malloc();
  }
  res = disk_write(blocks[block_index] + offset, buf,
                   min(loaded_xfs.block_size - offset, nbyte));
  if (res == -1)
    return -1;
  nbyte -= res;
  read_bytes += res;
  block_index++;

  while (nbyte >= loaded_xfs.block_size) { // 读整块的部分
    if (blocks[block_index] == DA_NULL) {
      blocks[block_index] = block_malloc();
    }
    res = disk_write(blocks[block_index], buf, loaded_xfs.block_size);
    if (res == -1)
      return -1;
    nbyte -= res;
    read_bytes += res;
    block_index++;
  }
  if (nbyte > 0) { // 读剩余部分
    if (blocks[block_index] == DA_NULL) {
      blocks[block_index] = block_malloc();
    }
    res = disk_write(blocks[block_index], buf, nbyte);
    if (res == -1)
      return -1;
    nbyte -= res;
    read_bytes += res;
  }
  return read_bytes;
}
int _indirect_zone1_write(struct inode_struct *inode, diskptr_t indir_block,
                          char *buf, xsize_t nbyte, xsize_t offset);

static void _write_back(diskptr_t block_table, char *buf,
                        xsize_t size) { // commit changes
  if (disk_write(block_table, buf, size) == -1) {
    // FATEL ERROR
    exit(-1);
  }
}

static void _rollback(diskptr_t *blocks, diskptr_t *blocks_backup,
                      xsize_t len) {
  int i;
  for (i = 0; i < len; i++) {
    if (blocks[i] != blocks_backup[i]) {
      block_free(blocks[i]);
      blocks[i] = blocks_backup[i];
    }
  }
}

int _indirect_zone2_write(struct inode_struct *inode, char *buf, xsize_t nbyte,
                          xsize_t offset) {
  xsize_t indir_block1_max = loaded_xfs.idz_per_inode * loaded_xfs.block_size;
  diskptr_t *blocks = malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t));
  diskptr_t *blocks_backup =
      malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t)); // 原子化保障
  int res, read_bytes = 0;
  int i = offset / loaded_xfs.block_size;
  bool flag_need_write_back = false;

  offset = offset % loaded_xfs.block_size;
  if (inode->indir_block2 == DA_NULL) {
    inode->indir_block2 = block_malloc();
  }
  if (disk_read(inode->indir_block2, blocks, loaded_xfs.block_size) ==
      -1) { // 加载二级间接块的第一层
    free(blocks);
    return -1;
  }
  memcpy(blocks_backup, blocks, loaded_xfs.idz_per_inode * sizeof(diskptr_t));
  if (blocks[i] == DA_NULL) {
    flag_need_write_back = true;
    blocks[i] = block_malloc();
  }
  // 把二级间接块第二层当做一级间接块处理
  res = _indirect_zone1_write(
      NULL, blocks[i], buf, min(loaded_xfs.block_size - offset, nbyte), offset);
  if (res == -1) {
    if (flag_need_write_back) {
      _rollback(blocks, blocks_backup, loaded_xfs.idz_per_inode);
    }
    free(blocks);
    return -1;
  }
  read_bytes += res;
  nbyte -= res;
  buf += res;
  i++;
  // 读取整层的
  while (nbyte >= indir_block1_max) {
    if (blocks[i] == DA_NULL) {
      flag_need_write_back = true;
      blocks[i] = block_malloc();
    }
    res = _indirect_zone1_write(NULL, blocks[i], buf, indir_block1_max, 0);
    if (res == -1) {
      if (flag_need_write_back) {
        _rollback(blocks, blocks_backup, loaded_xfs.idz_per_inode);
      }
      free(blocks);
      return -1;
    }
    read_bytes += res;
    buf += res;
    nbyte -= res;
    i++;
  }
  if (nbyte > 0) {
    if (blocks[i] == DA_NULL) {
      flag_need_write_back = true;
      blocks[i] = block_malloc();
    }
    res = _indirect_zone1_write(NULL, blocks[i], buf, nbyte, 0);
    if (res == -1) {
      if (flag_need_write_back) {
        _rollback(blocks, blocks_backup, loaded_xfs.idz_per_inode);
      }
      free(blocks);
      return -1;
    }
    read_bytes += res;
  }
  if (flag_need_write_back) {
    _write_back(inode->indir_block2, blocks, loaded_xfs.block_size);
  }
  free(blocks);
  return read_bytes;
}

int _indirect_zone1_write(struct inode_struct *inode, diskptr_t indir_block,
                          char *buf, xsize_t nbyte, xsize_t offset) {
  xsize_t indir_block1_max = loaded_xfs.idz_per_inode * loaded_xfs.block_size;
  diskptr_t *blocks = malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t));
  if (disk_read(indir_block, blocks, loaded_xfs.block_size) ==
      -1) // 加载间接块表
    return -1;
  if (offset + nbyte < indir_block1_max) { // 情况1: 全部在一级间接块
    int res;
    diskptr_t *blocks_backup =
        malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t));
    memcpy(blocks_backup, blocks, loaded_xfs.idz_per_inode * sizeof(diskptr_t));
    res = _direct_zone_write(blocks, buf, nbyte, offset);
    if (res == -1) {
      _rollback(blocks, blocks_backup, loaded_xfs.idz_per_inode);
    }
    free(blocks);
    free(blocks_backup);
    return res;
  } else if (!inode) { // 如果inode没有给定，不用执行之后的
    free(blocks);
    return -1;
  } else if (offset < indir_block1_max) {
    // 情况2: 部分在一级间接块，部分在二级间接块
    int res2, res1 = _direct_zone_write(blocks, buf, indir_block1_max - offset,
                                        offset);
    diskptr_t *blocks_backup =
        malloc(loaded_xfs.idz_per_inode * sizeof(diskptr_t));
    memcpy(blocks_backup, blocks, loaded_xfs.idz_per_inode * sizeof(diskptr_t));
    res1 = _direct_zone_write(blocks, buf, nbyte, offset);
    nbyte -= res1;
    if (res1 == -1 || !inode) {
      _rollback(blocks, blocks_backup, loaded_xfs.idz_per_inode);
      free(blocks_backup);
      free(blocks);
      return res1;
    }
    res2 = _indirect_zone2_write(inode, buf + res1, nbyte, 0);
    if (res2 == -1) {
      free(blocks);
      return res2;
    }
    free(blocks);
    return res1 + res2;
  } else { // 情况3: 全部在二级间接块
    int res =
        _indirect_zone2_write(inode, buf, nbyte, offset - indir_block1_max);
    free(blocks);
    return res;
  }
}
// 提供对inode索引的原子性操作，但不能保证磁盘数据不被修改
// 如果要保证磁盘数据不被修改，应该有日志机制
int _write_inode(struct inode_struct *inode, char *buf, xsize_t nbyte,
                 xsize_t offset) {
  xsize_t direct_max_size;
  xsize_t old_file_size = inode->file_size;
  int res;
  if (nbyte + offset > inode->file_size) {
    if (nbyte + offset > loaded_xfs.max_filesize) {
      return -1;
    }
    inode->file_size = nbyte + offset;
  }
  direct_max_size = loaded_xfs.dz_per_inode * loaded_xfs.block_size;
  if (offset + nbyte < direct_max_size) { // 情况1: 全部在直接块区
    diskptr_t *block_backup =
        malloc(sizeof(diskptr_t) * loaded_xfs.dz_per_inode);
    memcpy(block_backup, inode->block,
           sizeof(diskptr_t) * loaded_xfs.dz_per_inode);
    res = _direct_zone_write(inode->block, buf, nbyte, offset);
    if (res == -1) {
      inode->file_size = direct_max_size;
      _rollback(inode->block, block_backup, loaded_xfs.dz_per_inode);
    }
    free(block_backup);
    return res;
  } else if (offset < direct_max_size) { // 情况2: 部分在直接块区，部分间接块区
    int res1, res2;
    diskptr_t *block_backup =
        malloc(sizeof(diskptr_t) * loaded_xfs.dz_per_inode);
    memcpy(block_backup, inode->block,
           sizeof(diskptr_t) * loaded_xfs.dz_per_inode);
    res1 =
        _direct_zone_write(inode->block, buf, direct_max_size - offset, offset);
    if (res1 == -1) {
      inode->file_size = direct_max_size;
      _rollback(inode->block, block_backup, loaded_xfs.dz_per_inode);
      free(block_backup);
      return -1;
    }
    nbyte -= res1;
    if (inode->indir_block == DA_NULL) {
      inode->indir_block = block_malloc();
    }
    res2 = _indirect_zone1_write(inode, inode->indir_block, buf, nbyte, 0);
    if (res2 == -1) {
      inode->file_size = direct_max_size;
      _rollback(inode->block, block_backup, loaded_xfs.dz_per_inode);
      free(block_backup);
      return -1; // 这里是返回之前部分还是不返回
    }
    free(block_backup);
    return res1 + res2;
  } else { // 情况3: 全部在间接块区
    if (inode->indir_block == DA_NULL) {
      inode->indir_block = block_malloc();
    }
    return _indirect_zone1_write(inode, inode->indir_block, buf, nbyte,
                                 offset - direct_max_size);
  }
}

xsize_t xfs_write(int fildes, char *buf, xsize_t nbyte) {
  struct fd_struct *fd = fd_table_search(fildes);
  if (fd == NULL)
    return -1;
  if (!(fd->mod & O_ACCMODE == O_WRONLY || fd->mod & O_ACCMODE == O_RDWR)) {
    return -1;
  }
  return _write_inode(fd->inode, buf, nbyte, fd->offset);
}
