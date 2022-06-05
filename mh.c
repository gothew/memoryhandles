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

static bool add_branch(struct mh *mh) {
  if (mh->pool_branches.cur == mh->pool_branches.len) {
    return false;
  }

  struct mh_branch *new_branch;
  new_branch = ((struct mh_branch *)mh->pool_branches.buf);
  new_branch += mh->pool_branches.cur;

  new_branch->elements = ((uint8_t *)mh->pool_elements.buf);
  new_branch->elements += mh->pool_elements.cur * mh->sizeof_element;

  new_branch->parent = NULL;
  new_branch->siblings = NULL;
  new_branch->children = NULL;
  new_branch->children_len = 0;
  new_branch->element_len = 0;
  ++(mh->pool_branches.cur);

  if (link_branch(mh, new_branch) == false) {
    return false;
  }

  mh->selected_branch = new_branch;
  mh->selected_element = 0;

  return true;
}

static bool rm_branch(struct mh *mh, struct mh_branch *selected_branch,
                      void *ptr) {
  struct mh_branch *parent = selected_branch->parent;
  bool run = true;

  if (selected_branch == mh->selected_branch) {
    struct mh_branch *process = parent->children;

    if (process == selected_branch) {
      parent->children = selected_branch->siblings;
    } else {
      while (process->siblings != selected_branch) {
        process = process->siblings;
      }

      process->siblings = selected_branch->siblings;
    }

    --(parent->children_len);
    run = false;
  }

  if (mh->pool_holes.cur == mh->pool_holes.len) {
    run = false;
  }

  struct mh_hole *new_hole;
  new_hole = (struct mh_hole *)mh->pool_holes.buf;
  new_hole += mh->pool_holes.cur;

  uint16_t element_offset =
      selected_branch->elements - ((uint8_t *)mh->pool_elements.buf);
  new_hole->id = element_offset / mh->sizeof_element;
  new_hole->len = selected_branch->element_len;
  ++(mh->pool_holes.cur);

  struct mh_branch *last_branch;
  last_branch = (struct mh_branch *)mh->pool_branches.buf;
  last_branch += mh->pool_branches.cur - 1;

  if (selected_branch != last_branch) {
    struct mh_branch *process = parent->children;

    memcpy(selected_branch, last_branch, sizeof(struct mh_branch));

    if (process == last_branch) {
      parent->children = selected_branch;
    } else {
      while (process->siblings != selected_branch) {
        process = process->siblings;
      }
      process->siblings = selected_branch;
    }
  }

  --(mh->pool_branches.cur);
  return run;
}

void mh_free(struct mh *mh) {
  if (mh->pool_elements.dyn == true) {
    free(mh->pool_elements.buf);
  }

  if (mh->pool_branches.dyn == true) {
    free(mh->pool_branches.buf);
  }
}

bool mh_init(struct mh *mh, void *buf_branches, uint16_t len_branches,
             uint16_t alloc_step_len_branches,
             uint16_t alloc_maximum_len_brances, void *buf_elements,
             uint16_t len_elements, uint16_t alloc_step_len_elements,
             uint16_t alloc_maximum_len_elementsm, uint16_t sizeof_element,
             void *buf_holes, uint16_t len_holes, uint16_t alloc_step_len_holes,
             uint16_t alloc_maximum_len_holes) {
  if (!alloc(&buf_branches, len_branches, sizeof(struct mh_branch))) {
    return false;
  }

  if (!alloc(&buf_holes, len_holes, sizeof(struct mh_hole))) {
    return false;
  }

  // Create pools
  mh->pool_branches.dyn = (buf_branches == NULL);
  mh->pool_branches.buf = buf_branches;
  mh->pool_branches.cur = 0;
  mh->pool_branches.len = len_branches;
  mh->pool_branches.maximum_len = alloc_maximum_len_brances;

  mh->pool_elements.dyn = (buf_elements == NULL);
  mh->pool_elements.buf = buf_elements;
  mh->pool_elements.cur = 0;
  mh->pool_elements.step_len = alloc_step_len_elements;
  mh->pool_elements.maximum_len = alloc_maximum_len_elementsm;
  mh->sizeof_element = sizeof_element;

  mh->pool_holes.dyn = (buf_holes == NULL);
  mh->pool_holes.buf = buf_holes;
  mh->pool_holes.cur = 0;
  mh->pool_holes.len = len_holes;
  mh->pool_holes.step_len = alloc_step_len_holes;
  mh->pool_holes.maximum_len = alloc_maximum_len_holes;

  // create root
  mh->selected_branch = (struct mh_branch *)mh->pool_branches.buf;
  mh->selected_branch->parent = NULL;
  mh->selected_branch->siblings = NULL;
  mh->selected_branch->children = NULL;
  mh->selected_branch->children_len = 0;
  mh->selected_branch->element_len = 0;

  // register root
  mh->root = mh->selected_branch;
  mh->pool_branches.cur = 1;
  mh->selected_element = 0;
  return true;
}
