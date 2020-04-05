#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "common.h"

#define printflog(format, ...) \
  do { \
    printf(format, ## __VA_ARGS__); \
    fflush(stdout); \
  } while (0)

#define Log(format, ...) \
    printflog("\33[1;33m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define assert(cond) \
    do { \
      if (!(cond)) { \
        printf("\033[33mAssertion fail at %s:%d\n\033[0m", __FILE__, __LINE__); \
        exit(1); \
      } \
    } while (0)

// #define panic(format, ...) \
//   assert(0, format, ## __VA_ARGS__)

#define TODO() Log("Please implement me."); \
  assert(0)

#endif