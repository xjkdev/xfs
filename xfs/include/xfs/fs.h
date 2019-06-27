#ifndef XFS_FS_H
#define XFS_FS_H
#include "include/xfs.h"

#ifndef O_CREAT
#define O_CREAT 0x0200   /* create if nonexistant */
#define O_TRUNC 0x0400   /* truncate to zero length */
#define O_EXCL 0x0800    /* error if already exists */
#define O_RDONLY 0x0000  /* open for reading only */
#define O_WRONLY 0x0001  /* open for writing only */
#define O_RDWR 0x0002    /* open for reading and writing */
#define O_ACCMODE 0x0003 /* mask for above modes */
#define O_APPEND 0x0008  /* set append mode */
#endif

#ifndef SEEK_SET
#define SEEK_SET 0 /* set file offset to offset */
#define SEEK_CUR 1 /* set file offset to current plus offset */
#define SEEK_END 2 /* set file offset to EOF plus offset */
#endif

#ifndef FM_IRWXU
#define FM_IRWXU 00700 /* RWX mask for owner */
#define FM_IRUSR 00400 /* R for owner */
#define FM_IWUSR 00200 /* W for owner */
#define FM_IXUSR 00100 /* X for owner */

#define FM_IRWXG 00070 /* RWX mask for group */
#define FM_IRGRP 00040 /* R for group */
#define FM_IWGRP 00020 /* W for group */
#define FM_IXGRP 00010 /* X for group */

#define FM_IRWXO 00007 /* RWX mask for other */
#define FM_IROTH 00004 /* R for other */
#define FM_IWOTH 00002 /* W for other */
#define FM_IXOTH 00001 /* X for other */
#endif

#define FM_DIR 01000

#define DEFAULT_MODE (FM_IRUSR | FM_IWUSR | FM_IRGRP | FM_IROTH)
#define DEFAULT_DIR_MODE                                                       \
  (FM_DIR | FM_IRWXU | FM_IRGRP | FM_IXGRP | FM_IROTH | FM_IXOTH)

int xfs_open(const char *path, int oflag);
int xfs_close(int fildes);
int xfs_remove(const char *path);
int xfs_fsync(int fildes);
int xfs_creat(const char *path, xmode_t mode);

xsize_t xfs_read(int fildes, char *buf, xsize_t nbyte);
xsize_t xfs_write(int fildes, const char *buf, xsize_t nbyte);
xoff_t xfs_lseek(int fildes, xoff_t offset, int whence);

int xfs_rename(const char *old, const char *new);

int xfs_mkdir(const char *path);
int xfs_rmdir(const char *path);

int xfs_chmod(const char *path, xmode_t mode);
int xfs_chown(const char *path, xuid_t owner, xgid_t group);

int xfs_login(const char *username, const char *passwd);
int xfs_logout();
int xfs_useradd(const char *username);
int xfs_passwd(xuid_t uid, const char *passwd);

xuid_t xfs_getuid();
xuid_t xfs_getgid();
int setgid(xgid_t gid);

struct XDIR_struct;
typedef struct XDIR_struct XDIR;
XDIR *xfs_opendir(const char *filename);

int xfs_format();
int xfs_load();
#endif
