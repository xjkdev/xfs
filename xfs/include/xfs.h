#ifndef XFSDEF_H
#define XFSDEF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int16_t xuid_t;
typedef int16_t xgid_t;
typedef int16_t xmode_t;
typedef int32_t xoff_t;
typedef uint32_t xsize_t;

typedef uint32_t diskptr_t;
typedef xsize_t disksize_t;

#define DA_NULL 0 // disk address null

struct list_head {
  struct list_head *next, *prev;
};

struct hlist_head {
  struct hlist_node *first;
};

struct hlist_node {
  struct hlist_node *next, **pprev;
};

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })

#endif
