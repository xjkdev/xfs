#include <include/disk.h>
#include <include/globals.h>
#include <stdio.h>

// fetal error: return -1, normal error: return 0
// or return size
int disk_write_super(diskptr_t addr, char *buff, xsize_t len) {
  if (fseek(disk_head.disk_file, sizeof(disk_head) + addr, 0) == -1) {
    perror("out of disk\n");
    return 0;
  }
  if (addr + len > disk_head.max_disk_size) {
    len = disk_head.max_disk_size - addr;
  }
  return fwrite(buff, sizeof(char), len, disk_head.disk_file);
}

// fetal error: return -1, normal error: return 0
// or return size
int disk_write(diskptr_t addr, char *buff, xsize_t len) {
  // printf("disk_write %xd %d\n", addr, len);
  if (addr < 512) {
    perror("permission denied\n");
    return -1;
  }
  return disk_write_super(addr, buff, len);
}
