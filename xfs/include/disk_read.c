#include"disk.h"
#include<stdio.h>

//error: return 0
//or return size
int32_t disk_read(int32_t addr, char *buff, int32_t len)
{
	if (fseek(disk_head.disk_file, sizeof(disk_head) + addr, 0) == -1)
	{
		perror("--------------out of disk--------------\n");
		return 0;
	}
	int32_t read_len = 0;
	//each time read a byte
	while (len--&&feof(disk_head.disk_file)==0/*still have space to read*/)
	{
		++read_len;
		fread(buff, sizeof(char), 1, disk_head.disk_file);
	}
	return read_len;
}