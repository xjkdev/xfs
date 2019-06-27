#include <assert.h>
#include <include/bitmap.h>
#include <include/disk.h>
#include <include/hashtable.h>
#include <include/list.h>
#include <include/rbtree.h>
#include <include/xfs/fs_types.h>
#include <stdio.h>
#include <stdlib.h>

void init_globals();

int main() {
  unsigned char bitmap[10] = {0};
  int i;
  init_globals();
  bitmap_setbit(bitmap, 1);
  bitmap_setbit(bitmap, 8);
  for (i = 0; i < 10; i++) {
    printf("%02x ", bitmap[i]);
  }
  printf("\n");
  printf("%d\n", (int)sizeof(struct inode_struct));
  return 0;
}
