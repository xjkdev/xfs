#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef EXTERN_HERE
#define EXTERN extern
#else
#define EXTERN
#endif

#include <include/rbtree.h>
#include <include/xfs.h>
// #include <include/xfs/fs_types.h>

EXTERN struct superblock_struct loaded_xfs;
EXTERN xuid_t cur_uid;
EXTERN xgid_t cur_gid;

EXTERN struct rb_root fd_table;
EXTERN int free_fd;
#endif
