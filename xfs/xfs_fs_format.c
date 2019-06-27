#include <include/bitmap.h>
#include <include/globals.h>
#include <include/list.h>
#include <include/xfs.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <stdlib.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

int disk_read(diskptr_t a, char *buf, disksize_t l) { return 0; }
int disk_write(diskptr_t a, char *buf, disksize_t l) { return 0; }
int disk_write_super(diskptr_t a, char *buf, disksize_t l) { return 0; }
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

void init_superblock(struct superblock_struct *sblock) {
  sblock->inode_count = 4096;
  sblock->inode_bitmap_count = sblock->inode_count / (1 << 15);
  sblock->block_count = 1 << 20;                                // 块个数
  sblock->block_bitmap_count = sblock->block_count / (1 << 15); // 块位图个数
  // superblock | inode_bitmap | block_bitmap | inodes | data
  sblock->first_data_block = INODE_SEG +
                             sblock->inode_bitmap_count * BLOCK_SIZE +
                             sblock->block_bitmap_count * BLOCK_SIZE +
                             sblock->inode_count * sizeof(struct inode_struct);

  // -- sblock->lb = 3;                  // 1个区段有8个块

  sblock->max_filesize = 0xFFFFFFFF - sblock->first_data_block;

  sblock->_magic_number = MAGIC_NUMBER;

  sblock->block_size = BLOCK_SIZE;
}

int xfs_format() {
  struct superblock_struct sblock;
  unsigned char emptyblock[4096] = {0};
  int i;
  struct inode_sturct root;
  diskptr_t root_ptr = INODE_SEG + sblock->inode_bitmap_count * BLOCK_SIZE +
                       sblock->block_bitmap_count * BLOCK_SIZE;

  init_superblock(&sblock);
  disk_write_super(0x00000000, (char *)&sblock, sizeof(sblock));

  blockset(INODE_SEG, 0, sblock.inode_bitmap_count);
  blockset(INODE_SEG + sblock.inode_bitmap_count * BLOCK_SIZE, 0,
           sblock.block_bitmap_count);

  bitmap_setbit(&emptyblock, 0); // only root inode is used
  disk_write(INODE_SEG, &emptyblock, 4096);
  init_inode(&root, 0, 0);
  root.mod = DEFAULT_DIR_MODE;
  disk_write(root_ptr, &root, sizeof(struct inode_struct));

  for (i = 0; i < sblock.first_data_block / 4096; i++)
    bitmap_setbit(&emptyblock, i);
  disk_write(INODE_SEG + sblock->inode_bitmap_count * BLOCK_SIZE, &emptyblock,
             4096);
  return 0;
}

int xfs_load() {
  diskptr_t first_inode;
  struct list_head *pos, *n;
  disk_read(0x0, (char *)&loaded_xfs, sizeof(loaded_xfs));
  first_inode = INODE_SEG + loaded_xfs.inode_bitmap_count * BLOCK_SIZE +
                loaded_xfs.block_bitmap_count * BLOCK_SIZE;

  if (loaded_xfs.root_inode == NULL)
    loaded_xfs.root_inode = malloc(sizeof(struct inode_struct));
  disk_read(0x0, (char *)&loaded_xfs.root_inode, sizeof(struct inode_struct));

  loaded_xfs.inode_mounted_to = NULL;
  loaded_xfs.inodes_per_block =
      loaded_xfs.block_size / sizeof(struct inode_struct);
  loaded_xfs.dz_per_inode = INODE_DIRECT_COUNT;
  loaded_xfs.idz_per_inode = loaded_xfs.block_size / sizeof(diskptr_t);

  return -1;
}
