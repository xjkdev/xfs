#include <include/globals.h>
#include <include/path.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <include/xfs/general.h>
#include <include/xfs/permission.h>
#include <stdlib.h>

int _direct_zone_write(diskptr_t blocks[], char *buf, xsize_t nbyte,
                       xsize_t offset);
int _indirect_zone1_write(struct inode_struct *inode, diskptr_t indir_block,
                          char *buf, xsize_t nbyte, xsize_t offset);
int _indirect_zone2_write(struct inode_struct *inode, char *buf, xsize_t nbyte,
                          xsize_t offset);
int _write_inode(struct inode_struct *inode, char *buf, xsize_t nbyte,
                 xsize_t offset);

int _direct_zone_read(diskptr_t blocks[], char *buf, xsize_t nbyte,
                      xsize_t offset);
int _indirect_zone2_read(struct inode_struct *inode, char *buf, xsize_t nbyte,
                         xsize_t offset);
int _indirect_zone1_read(struct inode_struct *inode, diskptr_t indir_block,
                         char *buf, xsize_t nbyte, xsize_t offset);
int _read_inode(struct inode_struct *inode, char *buf, xsize_t nbyte,
                xsize_t offset);
int _truncate_inode(struct inode_struct *inode, off_t length) {
  if (length >= 0) {
    return 0;
  } else if (-length < inode->size % loaded_xfs.block_size) {
    inode->file_size += length; // length is negtive
  } else {
    inode->file_size += length; // length is negtive
    // TODO: free extra block
  }
  return 0;
}
xsize_t xfs_read(int fildes, char *buf, xsize_t nbyte) { return -1; }

int _open_parent_dir(const char *path, struct inode_struct *parent,
                     char *filename) {
  struct inode_struct *cur_dir = loaded_xfs.root_inode;
  char **split = split_path(path, strlen(path));
  char *name, **pos;
  struct diritem_struct *iter_item, *dir_items = malloc(cur_dir->file_size);
  int res = _read_inode(cur_dir, (char *)dir_items, cur_dir->file_size, 0);

  path_split_for_each(name, pos, split) { // get parent inode
    bool flag_found = false;
    xsize_t cur_file_size;
    if (*(pos + 1) == NULL) // 后一个是空表示当前为要打开的文件名
      break;
    diritem_for_each(iter_item, dir_items,
                     cur_dir->file_size / sizeof(struct diritem_struct)) {
      if (strncmp(iter_item->filename, name, FILENAME_MAX_LENGTH) == 0) {
        flag_found = true;
        break;
      }
    }
    if (!flag_found) {
      // TODO: set errno
      free(dir_items);
      return -1;
    }

    cur_file_size = cur_dir->file_size;
    disk_read(iter_item->inode, (char *)parent, sizeof(struct inode_struct));
    if (!(parent->mod & FM_DIR)) {
      // 不是目录无法打开
      free(dir_items);
      return -1;
    }
    if (!check_permission_read(parent) || !check_permission_execute(parent)) {
      // 没有权限无法进入
      free(dir_items);
      return -1;
    }
    if (cur_file_size < parent->file_size) {
      dir_items = realloc(dir_items, parent->file_size);
    }

    cur_dir = parent;
  }
  if (filename != NULL) {
    strncpy(filename, name, 60);
  }
  destroy_path_split(split);
  free(dir_items);
  return 0;
}

int xfs_open(const char *path, int oflag) {
  struct inode_struct parent;
  char filename[60];
  struct inode_struct *opened_file = malloc(sizeof(struct inode_struct));
  struct fd_struct *new_fd;
  struct diritem_struct *iter_item, *dir_items;
  diskptr_t inode_ptr;
  _open_parent_dir(path, &parent, filename);
  dir_items = malloc(parent.file_size);
  _read_inode(&parent, dir_items, parent.file_size, 0);

  diritem_for_each(iter_item, dir_items,
                   parent.file_size / sizeof(struct diritem_struct)) {
    if (strncmp(iter_item->filename, filename, 60) == 0) {
      // 文件已存在
      flag_existed = true;
      break;
    }
  }
  if (flag_existed && (oflag & O_EXCL)) {
    free(dir_items);
    free(opened_file);
    return -1;
  }
  if (!flag_existed) {
    struct diritem_struct new_diritem;
    inode_ptr = inode_alloc();
    init_inode(opened_file, cur_uid, cur_gid);
    new_diritem.inode = inode_ptr;
    strncpy(new_diritem.filename, filename, 60);
    disk_write(inode_ptr, (char *)opened_file, sizeof(struct inode_struct));
    _write_inode(&parent, (char *)&new_diritem, sizeof(struct diritem_struct),
                 parent.file_size); // 新目录项追加在最后
  } else {
    inode_ptr = iter_item->inode;
    disk_read(inode_ptr, (char *)opened_file, sizeof(struct inode_struct));
  }
  new_fd = malloc(sizeof(struct fd_struct));
  init_fd_struct(new_fd);
  insert_fd_struct(new_fd);
  new_fd->inode = opened_file;
  new_fd->inode_ptr = inode_ptr;
  new_fd->oflags = oflag;
  if (oflag & O_APPEND) {
    new_fd->offset = opened_file->file_size;
  }
  free(dir_items);
  return 0;
}

void _free_indirect_block1(diskptr_t p) {
  int i;
  diskptr_t *tmp = malloc(loaded_xfs.block_size);
  disk_read(p, tmp, loaded_xfs.block_size);
  for (i = 0; i < loaded_xfs.idz_per_inode; i++) {
    if (tmp[i] != DA_NULL) {
      block_free(tmp[i]);
    }
  }
  free(tmp);
}

void _free_indirect_block2(diskptr_t p) {
  int i;
  diskptr_t *tmp = malloc(loaded_xfs.block_size);
  disk_read(p, tmp, loaded_xfs.block_size);
  for (i = 0; i < loaded_xfs.idz_per_inode; i++) {
    if (tmp[i] != DA_NULL) {
      _free_indirect_block1(tmp[i]);
    }
  }
  free(tmp);
}

void _clear_file_content(struct inode_struct *inode) {
  int i = 0;
  inode->file_size = 0;
  for (i = 0; i < loaded_xfs.dz_per_inode; i++) {
    if (inode->block[i] != DA_NULL) {
      block_free(inode->block[i]);
      inode->block[i] = DA_NULL;
    }
  }
  if (inode->indir_block != DA_NULL) {
    _free_indirect_block1(inode->indir_block);
    block_free(inode->indir_block);
    inode->indir_block = DA_NULL;
  }
  if (inode->indir_block2 != DA_NULL) {
    _free_indirect_block2(inode->indir_block2);
    block_free(inode->indir_block2);
    inode->indir_block2 = DA_NULL;
  }
}

int xfs_creat(const char *path, xmode_t mode) {
  struct inode_struct parent;
  struct diritem_struct *iter_item, *dir_items;
  char filename[60];
  bool flag_existed = false;
  if (_open_parent_dir(path, &parent, filename) == -1)
    return -1;
  dir_items = malloc(parent.file_size);
  _read_inode(&parent, dir_items, parent.file_size, 0);

  diritem_for_each(iter_item, dir_items,
                   parent.file_size / sizeof(struct diritem_struct)) {
    if (strncmp(iter_item->filename, filename, 60) == 0) {
      // 文件已存在
      struct inode_struct file;
      flag_existed = true;
      disk_read(iter_item->inode, (char *)&file, sizeof(struct inode_struct));
      _clear_file_content(&file);
      disk_write(iter_item->inode, (char *)&file, sizeof(struct inode_struct));
      break;
    }
  }
  if (!flag_existed) {
    struct inode_struct new_file;
    struct diritem_struct new_diritem;
    // TODO: init_inode()
    new_file->mod = mode;

    diskptr_t new_inode_ptr = inode_alloc();
    init_inode(&new_file, cur_uid, cur_gid);
    new_diritem.inode = new_inode_ptr;
    strncpy(new_diritem.filename, filename, 60);
    disk_write(new_inode_ptr, (char *)&new_file, sizeof(struct inode_struct));
    _write_inode(&parent, (char *)&new_diritem, sizeof(struct diritem_struct),
                 parent.file_size); // 新目录项追加在最后
  }
  free(dir_items);
  return 0;
}

static int _xfs_sync_internal(struct fd_struct *filedes) {
  return disk_write(filedes->inode_ptr, (char *)filedes->inode,
                    sizeof(struct inode_struct));
}

int xfs_fsync(int fildes) {
  struct fd_struct *pos = fd_table_search(fildes);
  if (pos != NULL)
    return _xfs_sync_internal(pos);
  else
    return -1;
}

int xfs_close(int fildes) {
  struct fd_struct *pos = fd_table_search(fildes);
  if (pos != NULL) {
    _xfs_sync_internal(pos);
    free(pos->inode);
  } else
    return -1;
}

int xfs_remove(const char *path) {
  struct inode_struct parent;
  struct diritem_struct *iter_item, *dir_items;
  char filename[60];
  int i = 0;
  bool flag_existed = false;
  if (_open_parent_dir(path, &parent, filename) == -1)
    return -1;
  dir_items = malloc(parent.file_size);
  _read_inode(&parent, dir_items, parent.file_size, 0);

  diritem_for_each(iter_item, dir_items,
                   parent.file_size / sizeof(struct diritem_struct)) {
    if (strncmp(iter_item->filename, filename, 60) == 0) {
      // 文件已存在
      flag_existed = true;
      break;
    }
    i++;
  }
  if (!flag_existed) {
    free(dir_items);
    return -1;
  } else {
    int total_n = parent.file_size / sizeof(struct diritem_struct);
    struct inode_struct file;
    disk_read(iter_item->inode, (char *)&file, sizeof(struct inode_struct));
    _clear_file_content(&file);
    if (total_n - (i + 1) != 0) {
      _write_inode(parent, iter_item + 1, total_n - (i + 1),
                   i * sizeof(struct diritem_struct));
    }
    _truncate_inode(inode, -sizeof(struct diritem_struct));
    return 0;
  }
}

int xfs_mkdir(const char *path) { return xfs_creat(path, DEFAULT_DIR_MODE); }

XDIR *xfs_opendir(const char *filename) {
  int fd = xfs_open(filename, O_RDONLY);
  struct fd_struct *pos = fd_table_search(fd);
  XDIR *res = malloc(pos->inode->file_size);
  if (xfs_read(fd, res, pos->inode->file_size) == -1)
    return NULL;
  else
    return res;
}
