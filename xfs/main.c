#include <assert.h>
#include <include/bitmap.h>
#include <include/disk.h>
#include <include/hashtable.h>
#include <include/list.h>
#include <include/rbtree.h>
#include <include/xfs/fs_types.h>
#include <stdio.h>
#include <stdlib.h>

void init_globals();

char ah, str[20], j;

char func_name[20];
int parameter_count = 0;
struct parameter_struct {
  int tag;
  char *str;
  int num;
} parameters[10];

char *handle_string() {
  int i, ch;
  char *buffer = (char *)malloc(100);
  if ((ch = getchar()) != '\"') {
    printf("error %d\n", __LINE__);
    free(buffer);
    return NULL;
  }
  i = 0;
  ch = getchar();
  while (ch != '\"' && ch != EOF && i < 100) {
    buffer[i++] = ch;
    ch = getchar();
  }
  buffer[i] = '\0';
  if (ch != '\"') {
    printf("error %d\n", __LINE__);
    free(buffer);
    return NULL;
  }
  // printf("%s", buffer);
  return buffer;
}

int handle_number() {
  int res = 0;
  int ch;
  ch = getchar();
  while ('0' <= ch && ch <= '9') {
    res = res * 10 + ch - '0';
    ch = getchar();
  }
  if (!('0' <= ch && ch <= '9')) {
    ungetc(ch, stdin);
  }
  return res;
}

void handle_parameter() {
  int peak_ch, ch;

  peak_ch = getchar();
  ungetc(peak_ch, stdin);
  if (peak_ch == '\"') {
    char *str = handle_string();
    struct parameter_struct tmp;
    tmp.tag = 0;
    tmp.str = str;
    parameters[parameter_count] = tmp;
    parameter_count++;
  } else if ('0' <= peak_ch && peak_ch <= '9') {
    int num = handle_number();
    struct parameter_struct tmp;
    tmp.tag = 1;
    tmp.num = num;
    parameters[parameter_count] = tmp;
    parameter_count++;
  }
  ch = getchar();
  if (ch == ',') {
    handle_parameter();
  } else if (ch == ')') {
    return;
  } else if (parameter_count >= 10) {
    printf("error reach max parameter\n");
    return;
  } else {
    printf("error %d\n", __LINE__);
    return;
  }
}

void handle_func() {
  char ch;
  int i = 0;
  ch = getchar();
  while ('a' <= ch && ch <= 'z' || ch == '_') {
    func_name[i++] = ch;
    ch = getchar();
  }
  func_name[i] = '\0';
  if (ch == '(') {
    parameter_count = 0;
    handle_parameter();
  } else {
    printf("error %d\n", __LINE__);
  }
}

int main() {
  init_globals();
  int ch;
  char buffer[200];
  while ((ch = getchar()) != EOF) {
    if (ch != '\r' && ch != '\n' && ch != ' ')
      ungetc(ch, stdin);
    handle_func();
    if (parameter_count == 0) {
      if (strcmp(func_name, "logout") == 0) {
        int res = xfs_logout();
        printf("%d\n", res);
      }
      if (strcmp(func_name, "disk_close") == 0) {
        int res = xfs_close();
        printf("%d\n", res);
      }
      if (strcmp(func_name, "format") == 0) {
        int res = xfs_format();
        printf("%d\n", res);
      }
      if (strcmp(func_name, "load") == 0) {
        int res = xfs_load();
        printf("%d\n", res);
      }
    } else if (parameter_count == 1) {
      if (strcmp(func_name, "remove") == 0) {
        int res = xfs_remove(parameters[0].str);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "disk_init") == 0) {
        int res = disk_init(parameters[0].str);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "disk_open") == 0) {
        int res = disk_open(parameters[0].str);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "fsync") == 0) {
        int res = xfs_fsync(parameters[0].num);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "close") == 0) {
        int res = xfs_close(parameters[0].num);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "mkdir") == 0) {
        int res = xfs_mkdir(parameters[0].str);
        printf("%d\n", res);
      }
    } else if (parameter_count == 2) {
      if (strcmp(func_name, "open") == 0) {
        int res = xfs_open(parameters[0].str, parameters[1].num);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "creat") == 0) {
        int res = xfs_creat(parameters[0].str, parameters[1].num);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "login") == 0) {
        int res = xfs_login(parameters[0].str, parameters[1].str);
        printf("%d\n", res);
      }
    } else if (parameter_count == 3) {
      if (strcmp(func_name, "read") == 0) {
        int res = xfs_read(parameters[0].num, buffer, parameters[2].num);
        printf("%d\n", res);
        printf("%200s\n", buffer);
      }
      if (strcmp(func_name, "write") == 0) {
        int res =
            xfs_write(parameters[0].num, parameters[1].str, parameters[2].num);
        printf("%d\n", res);
      }
    }
  }
  return 0;
}
