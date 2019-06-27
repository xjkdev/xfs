#ifndef XFS_GENERAL_H
#define XFS_GENERAL_H

diskptr_t block_alloc();
diskptr_t block_malloc();
int block_free(diskptr_t block);
int block_set(diskptr_t block, char val, xsize_t size);

diskptr_t inode_alloc();
int inode_free(diskptr_t inode);

void init_inode(struct inode_struct *inode, xuid_t uid, xgid_t gid);
void init_fd_struct(struct fd_struct *);
void init_superblock(struct superblock_struct *sblock);

int insert_fd_struct(struct fd_struct *filedes);
void remove_fd_struct(struct fd_struct *filedes);
struct fd_struct *fd_table_search(int fd);

#endif
