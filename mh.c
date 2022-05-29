#include "mh.h"
#include "def.h"
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

static bool count_branches(struct mh *mh, struct mh_branch *select_branch,
                           void *ptr) {
  union p32 count;

  count.vp32 = ptr;
  ++(*count.up32);
  return true;
}

static bool link_branch(struct mh *mh, struct mh_branch *new_branch) {
  struct mh_branch *parent = mh->selected_branch;

  if (mh->selected_element < (mh->selected_branch->element_len - 1)) {
    if (mh->pool_branches.cur == mh->pool_branches.len) {
      return false;
    }

    struct mh_branch *split_branch;
    split_branch = ((struct mh_branch *)mh->pool_branches.buf);
    split_branch += mh->pool_branches.cur;

    split_branch->parent = parent;
    split_branch->siblings = new_branch;
    split_branch->children = parent->children;
    split_branch->children_len = parent->children_len;

    split_branch->elements = parent->elements;
    split_branch->elements += (mh->selected_element + 1) * mh->sizeof_element;

    split_branch->element_len = parent->element_len - mh->selected_element - 1;
    ++(mh->pool_branches.cur);

    // update the original parent
    parent->children = split_branch;
    parent->children_len = 2;
    parent->element_len = mh->selected_element + 1;

    struct mh_branch *process = split_branch->children;

    while (process != NULL) {
      process->parent = split_branch;
      process = process->siblings;
    }
    new_branch->parent = parent;
  } else {
    new_branch->parent = parent;
    new_branch->siblings = parent->children;

    parent->children = new_branch;
    ++(parent->children_len);
  }
  return true;
}
