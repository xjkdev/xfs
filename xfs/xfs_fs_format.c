#include <include/globals.h>
#include <include/list.h>
#include <include/xfs.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <stdlib.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

int disk_read(diskptr_t a, char *buf, disksize_t l) { return 0; }
int disk_write(diskptr_t a, char *buf, disksize_t l) { return 0; }

/*
    硬盘空间     4G     0xFFFFFFFF
    最大文件大小  4G - 开始
    块大小       4k     0x00000FFF
    块个数       2^20   0x000FFFFF
    I结点大小           64字节
    一块i结点个数        8
    一块位图     2^15   32,768个位
 */

void init_superblock(struct superblock_struct *sblock) {
  sblock->inode_count = 4096;
  sblock->inode_bitmap_count = 64;
  sblock->block_bitmap_count = 5; // 块位图个数
  // superblock | inode_bitmap | block_bitmap | inodes | data
  sblock->first_data_block = INODE_SEG +
                             sblock->inode_bitmap_count * BLOCK_SIZE +
                             sblock->block_bitmap_count * BLOCK_SIZE +
                             sblock->inode_count * sizeof(struct inode_struct);

  // -- sblock->lb = 3;                  // 1个区段有8个块

  sblock->max_filesize = 0xFFFFFFFF - sblock->first_data_block;
  sblock->block_count = 0x000FFFFF; // 块个数
  sblock->_magic_number = MAGIC_NUMBER;

  sblock->block_size = BLOCK_SIZE;
}

int xfs_format() {
  struct superblock_struct sblock;
  char emptyblock[4096] = {0};

  init_superblock(&sblock);
  disk_write(0x00000000, (char *)&sblock, sizeof(sblock));
  // init_inode_bitmap
  // disk_write(INODE_SEG, emptyblock, 4096);
  // init_block_bitmap (set superblock and inode etc as non free)
  // add root inode
  return 0;
}

int xfs_load() {
  diskptr_t first_inode;
  struct list_head *pos, *n;
  disk_read(0x0, (char *)&loaded_xfs, sizeof(loaded_xfs));
  first_inode = INODE_SEG + loaded_xfs.inode_bitmap_count * BLOCK_SIZE +
                loaded_xfs.block_bitmap_count * BLOCK_SIZE;
  // 为什么要用链表存 ？
  list_for_each_safe(pos, n, &loaded_inodes) {
    struct inode_list *entry = list_entry(pos, struct inode_list, list);
    list_del(pos);
    free(entry);
  }

  struct inode_list *node = malloc(sizeof(struct inode_list));
  disk_read(0x0, (char *)&node->inode, sizeof(struct inode_struct));
  list_add(&node->list, &loaded_inodes);

  loaded_xfs.inode_mounted_to = NULL;
  loaded_xfs.inodes_per_block =
      loaded_xfs.block_size / sizeof(struct inode_struct);
  loaded_xfs.dz_per_inode = INODE_DIRECT_COUNT;
  loaded_xfs.idz_per_inode = loaded_xfs.block_size / sizeof(diskptr_t);

  return -1;
}
