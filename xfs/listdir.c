
#include <include/list.h>
#include <include/xfs/fs.h>
#include <stdio.h>
void listdir(const char *path) {
  XDIR *dirs = xfs_opendir(path);
  struct list_head *pos;
  if (!list_empty(&dirs->node)) {
    list_for_each(pos, &dirs->node) {
      printf("%s\n", list_entry(pos, XDIR, node)->item.filename);
    }
  }
}
