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
