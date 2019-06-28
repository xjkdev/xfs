#include <assert.h>
#include <include/bitmap.h>

int main() {
  unsigned char bitmap[10] = {0};
  int i;
  bitmap_setbit(bitmap, 1);
  bitmap_setbit(bitmap, 8);
  assert(bitmap[0] == 0x02);
  assert(bitmap[1] == 0x01);
  return 0;
}
