#include <assert.h>
#include <include/bitmap.h>
#include <include/disk.h>
#include <include/hashtable.h>
#include <include/list.h>
#include <include/rbtree.h>
#include <include/xfs/fs.h>
#include <include/xfs/fs_types.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

char *rl_buffer = NULL;
int rl_index = 0;
bool first_line = true;

int rl_ungetc() { rl_index--; }
const char *completion_all[] = {
    "disk_open(", "disk_init(", "format(",       "load()",
    "open(",      "close(",     "fsync(",        "read(",
    "write(",     "lseek(",     "listdir(",      "remove(",
    "mkdir(",     "rmdir(",     "chown(",        "chmod(",
    "stat(",      "login(",     "create_user(",  "logout()",
    "getuid()",   "getgid()",   "create_group(", "add_user_to_group(",
    NULL};

char *xfs_name_generator(const char *text, int state) {
  static int list_index, len;
  char *name;
  if (!state) {
    list_index = 0;
    len = strlen(text);
  }
  while ((name = completion_all[list_index++])) {
    if (strncmp(name, text, len) == 0) {
      return strdup(name);
    }
  }
  return NULL;
}

char **xfs_name_completion(const char *text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, xfs_name_generator);
}

int rl_getchar() {
  if (first_line) { // init
    read_history("/tmp/xfs_history");
    rl_attempted_completion_function = xfs_name_completion;
  }
  if (first_line || rl_buffer[rl_index] == '\0') {
    rl_buffer = readline("> ");
    if (rl_buffer)
      add_history(rl_buffer);
    rl_index = 0;
    first_line = false;
  }
  if (!rl_buffer)
    return EOF;
  else {
    return rl_buffer[rl_index++];
  }
}

void init_globals();

char ah, str[20], j;

char func_name[20];
int parameter_count = 0;
struct parameter_struct {
  int tag;
  char *str;
  int num;
} parameters[10];

char get_nonspace() {
  char ch = rl_getchar();
  while (ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t') {
    ch = rl_getchar();
  }
  return ch;
}

char *handle_string() {
  int i, ch;
  char *buffer = (char *)malloc(100);
  if ((ch = rl_getchar()) != '\"') {
    printf("error %d\n", __LINE__);
    free(buffer);
    return NULL;
  }
  i = 0;
  ch = rl_getchar();
  while (ch != '\"' && ch != EOF && i < 100) {
    buffer[i++] = ch;
    ch = rl_getchar();
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
  ch = get_nonspace();
  while ('0' <= ch && ch <= '9') {
    res = res * 10 + ch - '0';
    ch = get_nonspace();
  }
  if (!('0' <= ch && ch <= '9')) {
    rl_ungetc();
  }
  return res;
}

const struct const_repr {
  const char *str;
  int num;
} const_table[] = {{"O_CREAT", 0x0200},
                   {"O_TRUNC", 0x0400},
                   {"O_EXCL", 0x0800},
                   {"O_RDONLY", 0x0000},
                   {"O_WRONLY ", 0x0001},
                   {"O_RDWR", 0x0002},
                   {"O_ACCMODE", 0x0003},
                   {"O_APPEND", 0x0008},

                   {"SEEK_SET", 0},
                   {"SEEK_CUR", 1},
                   {"SEEK_END", 2},

                   {"FM_IRWXU", 00700},
                   {"FM_IRUSR", 00400},
                   {"FM_IWUSR", 00200},
                   {"FM_IXUSR", 00100},
                   {"FM_IRWXG", 00070},
                   {"FM_IRGRP", 00040},
                   {"FM_IWGRP", 00020},
                   {"FM_IXGRP", 00010},
                   {"FM_IRWXO", 00007},
                   {"FM_IROTH", 00004},
                   {"FM_IWOTH", 00002},
                   {"FM_IXOTH", 00001},
                   {NULL, 0}};

int handle_const() {
  char buffer[21];
  int i = 0, res;
  char ch;
  ch = rl_getchar();
  while ((('A' <= ch && ch <= 'Z') || ch == '_') && i < 20) {
    buffer[i] = ch;
    ch = rl_getchar();
  }
  buffer[i + 1] = '\0';
  struct const_repr *pos;
  bool flag_found = false;
  for (pos = const_table; pos->str != NULL; pos++) {
    if (strcmp(buffer, pos->str) == 0) {
      res = pos->num;
      flag_found = true;
    }
  }
  if (!flag_found) {
    res = 0;
    printf("error %d\n", __LINE__);
  }
  ch = get_nonspace();
  if (ch == '|') {
    res |= handle_const();
  }
  return res;
}

void handle_parameter() {
  int peak_ch, ch;

  peak_ch = get_nonspace();
  rl_ungetc();
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
  } else if ('A' <= peak_ch && peak_ch <= 'Z') {
    int num = handle_const();
    struct parameter_struct tmp;
    tmp.tag = 1;
    tmp.num = num;
    parameters[parameter_count] = tmp;
    parameter_count++;
  }
  ch = get_nonspace();
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
  ch = get_nonspace();
  while (('a' <= ch && ch <= 'z') || ch == '_') {
    func_name[i++] = ch;
    ch = rl_getchar();
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
  printf("Function Refrence:\n"
         "disk_open(real_world_path)\n"
         "disk_init(real_world_path)\n"
         "format()\n"
         "load()\n"

         "open(path,oflag) -> fd\n"
         "close(fd)\n"
         "fsync(fd)\n"
         "read(fd, {{buf}}, size) -> size    /* buf was preprepared in command "
         "line */\n"
         "write(fd, buf, size) -> size\n"
         "lseek(fd, offset, whence) -> offset\n"

         "listdir(path)\n"
         "remove(path)\n"
         "mkdir(path)\n"
         "rmdir(path)\n"

         "chown(path, uid, gid)\n"
         "chmod(path, mode)\n"
         "stat(path)\n"

         "login(name, passwd)\n"
         "create_user(name, passwd) -> uid\n"
         "logout()\n"
         "getuid() -> uid\n"
         "getgid() -> gid\n"

         "create_group(name) -> gid\n"
         "add_user_to_group(uid, gid)\n");
  printf("\n\n");
  printf("Const Reference:\n");
  struct const_repr *pos;
  for (pos = const_table; pos->str != NULL; pos++) {
    printf("%s\t%d\n", pos->str, pos->num);
  }
  printf("\n\n");
  init_globals();
  int ch;
  char buffer[200];
  while ((ch = rl_getchar()) != EOF) {
    if (ch != '\r' && ch != '\n' && ch != ' ')
      rl_ungetc();
    handle_func();
    if (parameter_count == 0) {
      if (strcmp(func_name, "logout") == 0) {
        int res = xfs_logout();
        printf("%d\n", res);
      }
      if (strcmp(func_name, "disk_close") == 0) {
        int res = disk_close();
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
      if (strcmp(func_name, "getgid") == 0) {
        int res = xfs_getgid();
        printf("%d\n", res);
      }
      if (strcmp(func_name, "getuid") == 0) {
        int res = xfs_getuid();
        printf("%d\n", res);
      }
    } else if (parameter_count == 1) {
      if (strcmp(func_name, "remove") == 0) {
        int res = xfs_remove(parameters[0].str);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "rmdir") == 0) {
        int res = xfs_rmdir(parameters[0].str);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "listdir") == 0) {
        xfs_listdir(parameters[0].str);
      }
      if (strcmp(func_name, "stat") == 0) {
        int res = xfs_stat(parameters[0].str);
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
      if (strcmp(func_name, "create_group") == 0) {
        int res = xfs_creat_group(parameters[0].str);
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
      if (strcmp(func_name, "create_user") == 0) {
        int res = xfs_creat_usr(parameters[0].str, parameters[1].str);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "add_user_to_group") == 0) {
        int res = add_usr_to_group(parameters[0].num, parameters[1].num);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "chmod") == 0) {
        int res = xfs_chmod(parameters[0].str, parameters[1].num);
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
      if (strcmp(func_name, "lseek") == 0) {
        int res =
            xfs_lseek(parameters[0].num, parameters[1].num, parameters[2].num);
        printf("%d\n", res);
      }
      if (strcmp(func_name, "chown") == 0) {
        int res =
            xfs_chown(parameters[0].str, parameters[1].num, parameters[2].num);
        printf("%d\n", res);
      }
    }
  }
  write_history("/tmp/xfs_history");
  return 0;
}
