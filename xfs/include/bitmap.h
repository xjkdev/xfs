#ifndef BITMAP_H
#define BITMAP_H
#include "xfs.h"

static inline bool bitmap_get(unsigned char *bitmap, size_t index) {
  bitmap += (index >> 3);
  return *bitmap & (1 << (index & 0x7));
}
static inline void bitmap_setbit(unsigned char *bitmap, size_t index) {
  bitmap += (index >> 3);
  *bitmap |= (1 << (index & 0x7));
}
static inline void bitmap_clearbit(unsigned char *bitmap, size_t index) {
  bitmap += (index >> 3);
  *bitmap &= ~(1 << (index & 0x7));
}
static inline void bitmap_notbit(unsigned char *bitmap, size_t index) {
  bool bit = bitmap_get(bitmap, index);
  if (bit) {
    bitmap_clearbit(bitmap, index);
  } else {
    bitmap_setbit(bitmap, index);
  }
}

#endif
