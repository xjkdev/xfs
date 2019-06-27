#include <include/disk.h>
#include <include/globals.h>
#include <stdio.h>
#include <string.h>

// error :return -1
// virtual_disk_head disk_head;
int disk_init(char *path) {
  FILE *f = fopen(path, "wb+");
  if (f == NULL) {
    perror("faile to init disk\n");
    return -1;
  }
  disk_head.head_size = sizeof(struct virtual_disk_head);
  disk_head.max_disk_size = DISKSIZE;
  disk_head.real_disk_size = DISKINITSIZE;
  disk_head.disk_file = f;
  memcpy(disk_head.disk_path, path, strlen(path));
  fwrite(&disk_head, sizeof(disk_head), 1, f);
  // init vdisk and write disk_head and (1024-sizeof(disk_head)) bytes into the
  // vdisk
  char ch[1024] = {'\0'};
  fwrite(&ch, sizeof(ch), 1024 - sizeof(disk_head), f);
  // fseek(f, 0, 0);
  fclose(f);
  /*
          deal with error
  */
  return 0;
}
