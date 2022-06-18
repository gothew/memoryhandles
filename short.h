#ifndef H_SHORT
#define H_SHORT

#include "def.h"
#include <stdbool.h>
#include <stdint.h>

void mh_branch_root(struct mh *mh);
bool mh_element_prev(struct mh *mh);
bool mh_branch_parent(struct mh *mh);
bool mh_branch_sibling(struct mh *mh);
bool mh_branch_child(struct mh *mh);
bool mh_element_set(struct mh *mh, uint16_t id);

#endif
