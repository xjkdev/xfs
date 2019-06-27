#include <include/disk.h>
#include <include/globals.h>
#include <stdio.h>

// error: return 0
// or return size
int disk_read(diskptr_t addr, const char *buff, xsize_t len) {
  if (fseek(disk_head.disk_file, sizeof(disk_head) + addr, 0) == -1) {
    perror("out of disk\n");
    return 0;
  }
  int read_len = 0;
  // if (addr + len > disk_head.max_disk_size) {
  //   len = disk_head.max_disk_size - addr;
  // }
  // printf("%d %d %d\n", addr, len, disk_head.real_disk_size);
  // if (addr + len > disk_head.real_disk_size) {
  //   // memset(buff + disk_head.real_disk_size - (addr + len), 0,
  //   //        addr + len - disk_head.real_disk_size);
  //   len = disk_head.real_disk_size - addr;
  //   read_len += disk_head.real_disk_size - addr;
  // }

  return fread(buff, sizeof(char), len, disk_head.disk_file);
}
