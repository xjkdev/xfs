#define EXTERN_HERE
#include <include/globals.h>
#undef EXTERN_HERE
#include <include/xfs/general.h>

void init_globals() {
  uid_auto_increase = 1;
  gid_auto_increase = 1;
  init_superblock(&loaded_xfs);

  RB_EMPTY_ROOT(&fd_table);
}
