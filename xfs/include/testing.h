#ifndef TESTING_H
#define TESTING_H
#include <assert.h>
#include <include/xfs.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#define print_res(res) printf("%s:%d %d\n", __FILE__, __LINE__, (res))
#else
#define print_res(res)
#endif

typedef void (*test_case_t)(void);
typedef void (*set_up_t)(void);
typedef void (*tear_down_t)(void);

void init_globals();

/*
 * run all test cases, using setup and teardown function
 * setup and teardown may be NULL pointer.
 */
#define run_all_tests(_test_cases, _setup, _teardown)                          \
  {                                                                            \
    set_up_t _set_up = _setup;                                                 \
    tear_down_t _tear_down = _teardown;                                        \
    int _i, _length = sizeof(_test_cases) / sizeof(test_case_t);               \
    for (_i = 0; _i < _length; _i++) {                                         \
      printf("Start testing %s, test case #%d\n", __FILE__, _i);               \
      if ((_set_up) != NULL)                                                   \
        (_set_up)();                                                           \
      if ((_test_cases)[_i] != NULL)                                           \
        (_test_cases)[_i]();                                                   \
      if ((_tear_down) != NULL)                                                \
        (_tear_down)();                                                        \
    }                                                                          \
    printf("All %d test cases pass in %s\n", _length, __FILE__);               \
  }

#endif
