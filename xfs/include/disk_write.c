#include"disk.h"
#include<stdio.h>

//error: return 0
//or return size
int32_t disk_write_super(int32_t addr, char *buff, int32_t len)
{
	return  disk_write( addr, buff,  len);
}

int32_t disk_write(int32_t addr, char *buff, int32_t len)
{
	if(addr<512)
		return 0;
	if (fseek(disk_head.disk_file, sizeof(disk_head) + addr, 0) == -1)
	{
		perror("--------------out of disk--------------\n");
		return 0;
	}
	
	//each time read a byte
	while (len--)
	{
		/*feof(disk_head.disk_file) == 0;*/
		//fread(buff, sizeof(char), 1, disk_head.disk_file);

		//each time write a byte
		fwrite(buff, sizeof(char), 1, disk_head.disk_file);
		//if (ferrr(disk_head.disk_file) != 0)
	}
	return len;
}