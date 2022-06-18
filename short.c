#include "short.h"
#include "def.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool mh_element_prev(struct mh *mh) {
  if (mh->selected_element > 0) {
    --(mh->selected_element);

    return true;
  }
  return false;
}

bool mh_branch_child(struct mh *mh) {
  if (mh->selected_branch->children != NULL) {
    mh->selected_branch = mh->selected_branch->children;

    return true;
  }
  return false;
}

bool mh_element_set(struct mh *mh, uint16_t id) {
  if (id < mh->selected_branch->element_len) {
    mh->selected_element = id;
    return true;
  }

  return false;
}

bool mh_branch_parent(struct mh *mh) {
  if (mh->selected_branch->parent != NULL) {
    mh->selected_branch = mh->selected_branch->parent;
    return true;
  }
  return false;
}
