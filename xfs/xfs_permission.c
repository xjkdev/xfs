#include <include/globals.h>
#include <include/xfs/fs.h>
#include <include/xfs/permission.h>

bool check_permission_read(struct inode_struct *inode) {
  if (cur_uid == inode->uid) { // same owner
    return inode->mod & FM_IRUSR;
  } else if (cur_gid == inode->gid) { // same group
    return inode->mod & FM_IRGRP;
  } else { // others
    return inode->mod & FM_IROTH;
  }
}

bool check_permission_write(struct inode_struct *inode) {
  if (cur_uid == inode->uid) { // same owner
    return inode->mod & FM_IWUSR;
  } else if (cur_gid == inode->gid) { // same group
    return inode->mod & FM_IWGRP;
  } else { // others
    return inode->mod & FM_IWOTH;
  }
}

bool check_permission_execute(struct inode_struct *inode) {
  if (cur_uid == inode->uid) { // same owner
    return inode->mod & FM_IXUSR;
  } else if (cur_gid == inode->gid) { // same group
    return inode->mod & FM_IXGRP;
  } else { // others
    return inode->mod & FM_IXOTH;
  }
}
