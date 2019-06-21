#pragma once
#include<stdint.h>
#define DISKSIZE 1024*1024*4//4GB
#define DISKHEADSIZE 128
#define DISKINITSIZE 1024 //1MB

typedef struct VirtualDisk_head
{
	uint32_t max_disk_size ;
	uint32_t real_disk_size;
	uint32_t head_size ;

}VirtualDisk_head;
//1024 bytes 
//typedef struct VirtualDisk
//{
//	VirtualDisk_head head;
//	char storage_are[DISKSIZE - sizeof(VirtualDisk_head)];
//
//
//}VirtualDisk;

int32_t disk_open(char *path,FILE*f);
int32_t disk_close(char*path,FILE*f);

int32_t disk_read(int32_t addr, char *buff, int32_t len);
int32_t disk_write(int32_t addr, char *buff, int32_t len);

int32_t disk_init(char *path,FILE*f);

