#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static char **split_path(const char *path, int length) {
  int i, j, last, count = 0;
  if (path[0] != '/') {
    count += 1;
  }
  for (i = 0; i < length; i++) {
    if (path[i] == '/') {
      count++;
    } else if (path[i] == '\0')
      break;
  }
  char **res = malloc(sizeof(char *) * (count + 1));
  j = 0;
  i = 0;
  if (path[0] == '/')
    i = 1;
  last = i;
  for (; i < length; i++) {
    if (path[i] == '/' || path[i] == '\0') {
      res[j] = malloc(i - last + 1);
      strncpy(res[j], path + i, i - last + 1);
      j++;
    }
    if (path[i] == '\0')
      break;
  }
  res[j] = NULL;
  return res;
}

static void destroy_path_split(char **split) {
  while (*split != NULL) {
    free(*split);
    split++;
  }
  free(split);
}

#define path_split_for_each(name, pos, split)                                  \
  for (pos = (split), name = *pos; *pos != NULL; pos++, name = *pos)

#endif
