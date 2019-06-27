#include"disk.h"
#include<stdio.h>


int32_t disk_close(FILE*f)
{
	/*
		something changed of disk_head
	*/
	disk_head.disk_file = NULL;
	return fclose(f);
}
