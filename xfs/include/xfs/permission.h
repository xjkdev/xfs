#ifndef XFS_PERMISSION_H
#define XFS_PERMISSION_H
#include <include/xfs.h>
#include <include/xfs/fs_types.h>

bool check_permission_read(struct inode_struct *inode);
bool check_permission_write(struct inode_struct *inode);
bool check_permission_execute(struct inode_struct *inode);
#endif
