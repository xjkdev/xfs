
#include <include/disk.h>
#include <include/globals.h>
#include <include/testing.h>

void init_globals();

void test_case1() {
  const char str[] = "hello world";
  char buffer[sizeof(str)];
  int res;
  res = disk_init("/tmp/a.vdisk");
  print_res(res);
  assert(res == 0);
  res = disk_open("/tmp/a.vdisk");
  print_res(res);
  assert(res == 0);

  res = disk_write(0x1000, str, sizeof(str));
  print_res(res);
  assert(res == sizeof(str));

  res = disk_read(0x1000, buffer, sizeof(str));
  print_res(res);
  assert(res == sizeof(str));

  res = strncmp(buffer, str, sizeof(str));
  print_res(res);
  assert(res == 0);

  res = disk_write(0x0, str, sizeof(str));
  print_res(res);
  assert(res == -1);

  res = disk_write_super(0x0, str, sizeof(str));
  print_res(res);
  assert(res == sizeof(str));

  res = disk_close();
  print_res(res);
  assert(res == 0);
}

test_case_t test_cases[] = {test_case1};

int main() {
  init_globals();
  run_all_tests(test_cases, NULL, NULL);
  return 0;
}
