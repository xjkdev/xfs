
#define DEBUG
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
  listdir("/");
  print_res(0);
  res = xfs_creat("/abcd", DEFAULT_MODE);
  print_res(res);
  assert(res == 0);
  listdir("/");
  print_res(0);
  res = xfs_creat("/ab", DEFAULT_MODE);
  print_res(res);
  listdir("/");
  print_res(0);
}

void test_mkdir() {
  int res;
  res = xfs_mkdir("/c");
  print_res(res);
  assert(res == 0);
  listdir("/");
  print_res(0);
  listdir("/c");
  print_res(0);
  res = xfs_mkdir("/c/a");
  print_res(res);
  assert(res == 0);
  listdir("/");
  print_res(0);
  listdir("/c");
  print_res(0);
  listdir("/c/a");
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

test_case_t test_cases[] = {test_format1, test_listdir, test_mkdir,
                            test_read_write};

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
