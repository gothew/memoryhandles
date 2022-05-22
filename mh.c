#include "mh.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef MH_STATIC
#include <stdlib.h>
#endif

union p32 {
  void *vp32;
  uint32_t *up32;
};

static bool alloc(void **buf, uint16_t len, uint16_t pointed_len) {
  if (*buf == NULL) {
#ifndef MH_STATIC
    *buf = malloc(len * pointed_len);

    if (*buf == NULL) {
      return false;
    }
#else
    return false;
#endif
  }
  return true;
}
