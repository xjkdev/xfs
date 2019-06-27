#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char **split_path(const char *path, int length) {
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
  char *tmp;
  for (; i <= length; i++) {
    if (path[i] == '/' || path[i] == '\0') {
      tmp = malloc(i - last + 1);
      strncpy(tmp, path + last, i - last);
      tmp[i - last] = '\0';
      res[j] = tmp;
      last = i + 1;
      j++;
    }
    if (path[i] == '\0')
      break;
  }
  res[j] = NULL;
  return res;
}

void destroy_path_split(char **split) {
  char **tmp = split;
  while (*tmp != NULL) {
    free(*tmp);
    tmp++;
  }
  free(split);
}

#define path_split_for_each(name, pos, split)                                  \
  for (pos = (split), name = *pos; *pos != NULL; pos++, name = *pos)

#endif
