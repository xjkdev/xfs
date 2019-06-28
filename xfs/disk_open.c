#include <include/disk.h>
#include <include/globals.h>
#include <stdio.h>

int disk_open(char *path) {
  FILE *f = fopen(path, "rb+");
  if (f == NULL) {
    perror("failed to open disk\n");
    return -1;
  }
  fread(&disk_head, sizeof(disk_head), 1, f);
  disk_head.disk_file = f;
  return 0;
}
