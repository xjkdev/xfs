#ifndef _XFS_DISK_H
#define _XFS_DISK_H

#include <include/disk_types.h>
#include <include/xfs.h>
// 1024 bytes
// typedef struct VirtualDisk
//{
//	virtual_disk_head head;
//	char storage_are[DISKSIZE - sizeof(virtual_disk_head)];
//
//
//}VirtualDisk;

// int32 disk_open(char *path,FILE*f);
// int32_t disk_close(char*path,FILE*f);

int disk_open(char *path);
int disk_close();

int disk_read(diskptr_t addr, const char *buff, xsize_t len);
int disk_write(diskptr_t addr, char *buff, xsize_t len);
int disk_write_super(diskptr_t addr, char *buff, xsize_t len);

int disk_init(char *path);

#endif
