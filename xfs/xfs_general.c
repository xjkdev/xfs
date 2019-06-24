#include <include/bitmap.h>
#include <include/globals.h>
#include <include/list.h>
#include <include/xfs/fs_types.h>
#include <include/xfs/general.h>
#include <stdlib.h>

static inline diskptr_t inode_bitmap_start() { return INODE_SEG; }
static inline diskptr_t block_bitmap_start() {
  xsize_t block_size = loaded_xfs.block_size;
  return INODE_SEG + block_size * loaded_xfs.inode_bitmap_count;
}
static inline diskptr_t inode_table_start() {
  xsize_t block_size = loaded_xfs.block_size;
  return INODE_SEG + block_size * (loaded_xfs.inode_bitmap_count +
                                   loaded_xfs.block_bitmap_count);
}

static int _disk_setzero(diskptr_t block) {
  char empty_buf[32] = {0};
  int i = 0;
  for (i = 0; i < loaded_xfs.block_size; i += 32) {
    if (disk_write(block + i, empty_buf, 32) == -1) {
      return -1;
    }
  }
  return 0;
}

diskptr_t block_alloc() {
  int bits_per_block = loaded_xfs.block_size * 8;
  unsigned char *bitmap = malloc(loaded_xfs.block_size);
  int i, j;
  bool flag_found = false;
  diskptr_t block_bitmap_ptr = block_bitmap_start();
  for (i = 0; i < loaded_xfs.block_bitmap_count;
       i++, block_bitmap_ptr += loaded_xfs.block_size) {
    int res = disk_read(block_bitmap_ptr, bitmap, loaded_xfs.block_size);
    if (res == -1) {
      free(bitmap);
      return DA_NULL;
    }
    for (j = 0; j < loaded_xfs.block_size; j++) {
      if (bitmap[j] != 0xff) {
        flag_found = true;
        break;
      }
    }
    if (flag_found)
      break;
  }
  if (flag_found) {
    diskptr_t res = loaded_xfs.first_data_block +
                    (bits_per_block * i + j * 8) * loaded_xfs.block_size;
    int k;
    for (k = 0; k < 8; k++) {
      if (bitmap_get(bitmap[j], k) == 0) {
        bitmap_setbit(bitmap[j], k);
        res += k * loaded_xfs.block_size;
        break;
      }
    }
    free(bitmap);
    return res;
  } else {
    free(bitmap);
    return DA_NULL;
  }
}

diskptr_t block_malloc() {
  diskptr_t res = block_alloc();
  if (res != DA_NULL)
    _disk_setzero(res);
  return res;
}

int block_free(diskptr_t block) {
  int bits_per_block = loaded_xfs.block_size * 8;
  int index = (block - loaded_xfs.first_data_block) / loaded_xfs.block_size;
  diskptr_t block_bitmap_ptr = block_bitmap_start() + index / 8;
  int bit_index = index % 8;
  unsigned char buf;
  int res = disk_read(block_bitmap_ptr, (char *)&buf, 1);
  if (res == -1)
    return -1;
  bitmap_clearbit(&buf, bit_index);
  res = disk_write(block_bitmap_ptr, (char *)&buf, 1);
  if (res == -1)
    return -1;
  return 0;
}

diskptr_t inode_alloc() {
  unsigned char *bitmap = malloc(loaded_xfs.block_size);
  int i, j;
  bool flag_found = false;
  diskptr_t inode_bitmap_ptr = inode_bitmap_start();
  for (i = 0; i < loaded_xfs.inode_bitmap_count;
       i++, inode_bitmap_ptr += loaded_xfs.block_size) {
    int res = disk_read(inode_bitmap_ptr, bitmap, loaded_xfs.block_size);
    if (res == -1) {
      free(bitmap);
      return DA_NULL;
    }
    for (j = 0; j < loaded_xfs.block_size; j++) {
      if (bitmap[j] != 0xff) {
        flag_found = true;
        break;
      }
    }
    if (flag_found)
      break;
  }
  if (flag_found) {
    diskptr_t res = inode_table_start() + 8 * loaded_xfs.block_size * i +
                    j * 8 * sizeof(struct inode_struct);
    int k;
    for (k = 0; k < 8; k++) {
      if (bitmap_get(bitmap[j], k) == 0) {
        bitmap_setbit(bitmap[j], k);
        res += k * sizeof(struct inode_struct);
        break;
      }
    }
    free(bitmap);
    return res;
  } else {
    free(bitmap);
    return DA_NULL;
  }
}

int inode_free(diskptr_t inode) {
  int bits_per_block = loaded_xfs.block_size * 8;
  int index = (inode - inode_table_start()) / sizeof(struct inode_struct);
  diskptr_t inode_bitmap_ptr = inode_bitmap_start() + index / 8;
  int bit_index = index % 8;
  unsigned char buf;
  int res = disk_read(inode_bitmap_ptr, (char *)&buf, 1);
  if (res != 1)
    return -1;
  bitmap_clearbit(&buf, bit_index);
  res = disk_write(inode_bitmap_ptr, (char *)&buf, 1);
  if (res != 1)
    return -1;
  return 0;
}
