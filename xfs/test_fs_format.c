
// #define DEBUG
#include <include/disk.h>
#include <include/globals.h>
#include <include/testing.h>
#include <include/xfs/fs.h>

void set_up() { int res; }

void tear_down() { int res; }

void test_format1() {
  int res;
  res = xfs_format();
  print_res(res);
  assert(res == 0);
  res = xfs_load();
  print_res(res);
  assert(res == 0);
  assert(loaded_xfs.root_inode != NULL);
}

void test_listdir() {
  int res;
  xfs_listdir("/");
  printf("Expect: null\n");
  print_res(0);
  res = xfs_creat("abcd", DEFAULT_MODE);
  print_res(res);
  assert(res == 0);
  xfs_listdir("/");
  printf("Expect: abcd\n");
  print_res(0);
  res = xfs_creat("/ab", DEFAULT_MODE);
  print_res(res);
  xfs_listdir("/");
  printf("Expect: abcd, ab\n");
  print_res(0);
}

void test_mkdir() {
  int res;
  res = xfs_mkdir("/c");
  print_res(res);
  assert(res == 0);
  xfs_listdir("/");
  printf("Expect: abcd, ab, c\n");
  print_res(0);
  xfs_listdir("/c");
  printf("Expect: null\n");
  print_res(0);
  res = xfs_mkdir("/c/a");
  print_res(res);
  assert(res == 0);
  xfs_listdir("/");
  printf("Expect: abcd, ab, c\n");
  print_res(0);
  xfs_listdir("/c");
  printf("Expect: a\n");
  print_res(0);
  xfs_listdir("/c/a");
  printf("Expect: null\n");
  print_res(0);
  xfs_listdir("/");
  printf("Expect: abcd, ab, c\n");
  print_res(0);
}

void test_read_write() {
  int res;
  char str1[] = "hello world";
  char buffer[sizeof(str1)];
  int fd = xfs_open("/abcd", O_RDWR);
  print_res(fd);
  assert(fd != -1);
  res = xfs_write(fd, str1, sizeof(str1));
  print_res(res);
  assert(res == sizeof(str1));
  res = xfs_close(fd);
  print_res(res);
  assert(res != -1);

  fd = xfs_open("/abcd", O_RDWR);
  print_res(fd);
  assert(fd != -1);
  res = xfs_read(fd, buffer, sizeof(str1));
  print_res(res);
  assert(res == sizeof(str1));
  res = strcmp(str1, buffer);
  print_res(res);
  assert(res == 0);
  res = xfs_close(fd);
  print_res(res);
  assert(res != -1);
}

void test_user() {
  int fd, res, a = xfs_creat_usr("abc", "123");
  print_res(a);
  res = xfs_login("abc", "123");
  print_res(res);
  assert(res == 1);
  assert(cur_uid = a);
  fd = xfs_open("/abcd", O_RDWR);
  print_res(fd);
  assert(fd == -1);
  res = xfs_creat("/defc", DEFAULT_MODE);
  print_res(res);
  assert(res == 0);
  res = xfs_creat("/opge", DEFAULT_MODE | FM_IRWXO);
  print_res(res);
  assert(res == 0);
  xfs_logout();
  res = xfs_open("/defc", O_RDWR);
  print_res(res);
  assert(res == -1);
  res = xfs_open("/opge", O_RDWR);
  print_res(res);
  assert(res == 0);
}

void test_remove() {
  xfs_listdir("/");
  printf("Expect: abcd, defc, opge, ab, c\n");
  int res = xfs_remove("/ab");
  print_res(res);
  assert(res == 0);
  xfs_listdir("/");
  printf("Expect: abcd, defc, opge, c\n");
  print_res(0);
}

test_case_t test_cases[] = {test_format1,    test_listdir, test_mkdir,
                            test_read_write, test_user,    test_remove};

int main() {
  const char str[] = "hello world";
  char buffer[sizeof(str)];
  init_globals();
  int res;
  res = disk_init("/tmp/a.vdisk");
  print_res(res);
  assert(res == 0);
  res = disk_open("/tmp/a.vdisk");
  print_res(res);
  assert(res == 0);

  run_all_tests(test_cases, NULL, NULL);

  res = disk_close();
  print_res(res);
  assert(res == 0);
  return 0;
}
