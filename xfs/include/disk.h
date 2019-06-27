#ifndef _XFS_DISK_H
#define _XFS_DISK_H



#include<stdint.h>
#include<stdio.h>
#define DISKSIZE 1024*1024*4//4GB
#define DISKHEADSIZE 128
#define DISKINITSIZE 1024 //1MB
#define DISKPATHLEN 100

typedef struct VirtualDisk_head
{
	uint32_t max_disk_size ;
	uint32_t real_disk_size;
	uint32_t head_size ;
	char disk_path[DISKPATHLEN];
	FILE *disk_file;

}VirtualDisk_head;

extern VirtualDisk_head disk_head;


//1024 bytes 
//typedef struct VirtualDisk
//{
//	VirtualDisk_head head;
//	char storage_are[DISKSIZE - sizeof(VirtualDisk_head)];
//
//
//}VirtualDisk;

//int32_t disk_open(char *path,FILE*f);
//int32_t disk_close(char*path,FILE*f);


int32_t disk_open(char *path);
int32_t disk_close(FILE*f);

int32_t disk_read(int32_t addr, char *buff, int32_t len);
int32_t disk_write(int32_t addr, char *buff, int32_t len);
int32_t disk_write_super(int32_t addr, char *buff, int32_t len);

int32_t disk_init(char *path);


#endif 