#include <include/disk.h>
#include <include/globals.h>
#include <stdio.h>
int disk_close() {
  FILE *f = disk_head.disk_file;
  disk_head.disk_file = NULL;
  return fclose(f);
}
