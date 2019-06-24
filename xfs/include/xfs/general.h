#ifndef XFS_GENERAL_H
#define XFS_GENERAL_H

diskptr_t block_alloc();
diskptr_t block_malloc();
int block_free(diskptr_t block);

diskptr_t inode_alloc();
int inode_free(diskptr_t inode);

#endif
