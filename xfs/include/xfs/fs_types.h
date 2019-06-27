#ifndef XFS_FS_TYPES
#define XFS_FS_TYPES
#include <include/xfs.h>
#include <include/xfs/fs.h>

#define MAGIC_NUMBER 0xFE9527EF
#define BLOCK_SIZE 4096
// #define INODE_SEG 0x0000001FF // after 512byte superblock
#define INODE_SEG 0x000000FFF // a block
#define FILENAME_MAX_LENGTH 60
#define INODE_DIRECT_COUNT 7
typedef unsigned char disk_block[BLOCK_SIZE];

// struct bootblock_struct {
//   char _unused[1024];
// };

struct inode_struct {
  xmode_t mod;
  uint16_t linked_count; // unused
  xuid_t uid;
  xgid_t gid;
  xsize_t file_size;
  xsize_t atime, mtime, ctime; // unused
  diskptr_t block[INODE_DIRECT_COUNT];
  diskptr_t indir_block;  // 1级间接节点
  diskptr_t indir_block2; // 2级间接节点
  diskptr_t _unused;      // 3级间接节点
};

struct superblock_struct {
  xsize_t inode_count;        // i结点个数
  xsize_t inode_bitmap_count; // i结点位图块个数
  xsize_t block_bitmap_count; // 块位图个数
  diskptr_t first_data_block; // 第一个数据块
  // xsize_t lb;                 // 块数除以区段Log2

  xsize_t max_filesize; // 最大文件大小
  xsize_t block_count;  // 块个数
  xsize_t _magic_number;

  xsize_t block_size;

  // struct {
  //   uint8_t major;
  //   uint8_t minor;
  //   uint16_t revision;
  // } version; -- 版本号

  // in memory
  struct inode_struct *root_inode;
  struct inode_struct *inode_mounted_to;
  int inodes_per_block;
  // -- 设备号
  // -- 版本号
  int dz_per_inode;  // direct zones per inodes
  int idz_per_inode; // indirect zones per indirect block
};

struct diritem_struct {
  diskptr_t inode;
  char filename[FILENAME_MAX_LENGTH];
};

struct inode_list {
  struct inode_struct inode;
  struct list_head list;
};

typedef struct diritem_struct XDIR_struct;

struct fd_struct {
  int fd;
  xoff_t offset;
  xmode_t oflags;
  diskptr_t inode_ptr;
  struct inode_struct *inode;
  struct rb_node node;
};

#define diritem_for_each(pos, items, length)                                   \
  for (pos = (items); pos < (items) + length; pos++)

#endif
