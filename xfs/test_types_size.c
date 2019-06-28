#include <assert.h>
#include <include/xfs/fs_types.h>

int main() {
  assert(sizeof(struct inode_struct) == 64);
  assert(sizeof(struct diritem_struct) == 64);
  return 0;
}
