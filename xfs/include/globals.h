#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef EXTERN_HERE
#define EXTERN extern
#else
#define EXTERN
#endif

#include <include/disk_types.h>
#include <include/permit.h>
#include <include/rbtree.h>
#include <include/xfs.h>
#include <include/xfs/fs_types.h>

EXTERN struct superblock_struct loaded_xfs;
EXTERN xuid_t cur_uid;
EXTERN xgid_t cur_gid;

EXTERN struct rb_root fd_table;
EXTERN int free_fd;
EXTERN struct virtual_disk_head disk_head;

EXTERN struct usr usr_list[USRMAXSIZE];
EXTERN struct group group_list[GROUPMAXSIZE];
EXTERN int16_t uid_auto_increase;
EXTERN int16_t gid_auto_increase;

#endif
