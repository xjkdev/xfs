#ifndef DISK_TYPES_H
#define DISK_TYPES_H
#include <stdio.h>

#define DISKSIZE 1024 * 1024 * 4 // 4GB
#define DISKHEADSIZE 128
#define DISKINITSIZE 1024 // 1MB
#define DISKPATHLEN 100

#include <include/xfs.h>

struct virtual_disk_head {
  xsize_t max_disk_size;
  xsize_t real_disk_size;
  xsize_t head_size;
  char disk_path[DISKPATHLEN];
  FILE *disk_file;
};
#endif
