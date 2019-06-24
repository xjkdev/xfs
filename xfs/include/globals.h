#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef EXTERN_HERE
#define EXTERN extern
#else
#define EXTERN
#endif

#include <include/xfs.h>
// #include <include/xfs/fs_types.h>

EXTERN struct superblock_struct loaded_xfs;
EXTERN struct list_head loaded_inodes;
EXTERN xuid_t cur_uid;
EXTERN xgid_t cur_gid;
#endif
