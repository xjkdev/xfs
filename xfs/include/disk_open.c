#include"disk.h"
#include<stdio.h>
int32_t disk_open(char *path)
{
	FILE*f = fopen(path, "ab+");
	if (f == NULL)
	{
		perror("--------------failed to open disk--------------\n");
		return 0;
	}
	fread(&disk_head, sizeof(disk_head), 1, f);
	disk_head.disk_file = f;
	return 1;
}