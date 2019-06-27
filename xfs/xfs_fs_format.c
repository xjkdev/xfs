#include <include/bitmap.h>
#include <include/disk.h>
#include <include/globals.h>
#include <include/list.h>
#include <include/xfs.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <include/xfs/general.h>
#include <stdlib.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

/*
    硬盘空间      4G 2^32
    最大文件大小   4G - 开始
    块大小        4k 2^12
    块个数        2^20个
    I结点大小                64字节
    一块i结点个数             64
    一块位图      2^15        32,768个位
    块位图大小    2^5         32块
 */

int xfs_format() {
  struct superblock_struct sblock;
  unsigned char emptyblock[4096] = {0};
  int i, res;
  struct inode_struct root;
  diskptr_t root_ptr;

  init_superblock(&sblock);
  root_ptr = INODE_SEG + sblock.inode_bitmap_count * BLOCK_SIZE +
             sblock.block_bitmap_count * BLOCK_SIZE;
  disk_write_super(0x00000000, (char *)&sblock, sizeof(sblock));

  block_set(INODE_SEG, 0, sblock.inode_bitmap_count);
  block_set(INODE_SEG + sblock.inode_bitmap_count * BLOCK_SIZE, 0,
            sblock.block_bitmap_count);

  bitmap_setbit((unsigned char *)emptyblock, 0); // only root inode is used
  disk_write(INODE_SEG, (char *)emptyblock, 4096);

  for (i = 0; i < sblock.first_data_block / 4096; i++)
    bitmap_setbit((unsigned char *)emptyblock, i);
  disk_write(INODE_SEG + sblock.inode_bitmap_count * BLOCK_SIZE,
             (char *)emptyblock, 4096);

  init_inode(&root, 0, 0);
  root.mod = DEFAULT_DIR_MODE;
  res = disk_write(root_ptr, (char *)&root, sizeof(struct inode_struct));
  printf("%d %d\n", res, root_ptr);
  return 0;
}

int xfs_load() {
  diskptr_t root_ptr;
  struct list_head *pos, *n;
  int res;
  res = disk_read(0x0, (char *)&loaded_xfs, sizeof(loaded_xfs));
  if (res == -1)
    return -1;
  if (loaded_xfs._magic_number != MAGIC_NUMBER)
    return -1;
  root_ptr = INODE_SEG + loaded_xfs.inode_bitmap_count * BLOCK_SIZE +
             loaded_xfs.block_bitmap_count * BLOCK_SIZE;
  // printf("%d\n", root_ptr);
  if (loaded_xfs.root_inode == NULL)
    loaded_xfs.root_inode = malloc(sizeof(struct inode_struct));
  res = disk_read(root_ptr, (char *)loaded_xfs.root_inode,
                  sizeof(struct inode_struct));
  printf("%d %d\n", res, root_ptr);
  loaded_xfs.inode_mounted_to = NULL;
  loaded_xfs.inodes_per_block =
      loaded_xfs.block_size / sizeof(struct inode_struct);
  loaded_xfs.dz_per_inode = INODE_DIRECT_COUNT;
  loaded_xfs.idz_per_inode = loaded_xfs.block_size / sizeof(diskptr_t);

  return 0;
}
