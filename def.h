#ifndef H_MH_DEF
#define H_MH_DEF

#include <stdbool.h>
#include <stdint.h>

struct mh_hole {
  uint16_t id;
  uint16_t len;
};

struct mh_branch {
  struct mh_branch *parent;
  struct mh_branch *siblings;

  struct mh_branch *children;
  uint8_t *elements;

  uint16_t children_len;
  uint16_t element_len;
};

// only fragments when deleting elements
struct mh_pool {
  bool dyn;
  void *buf;

  uint16_t cur;
  uint16_t len;

  uint16_t step_len;
  uint16_t maximum_len;
};

struct mh {
  struct mh_pool pool_elements;
  struct mh_pool pool_branches;
  struct mh_pool pool_holes;
  uint16_t sizeof_element;

  struct mh_branch *root;
  struct mh_branch *selected_branch;
  uint16_t selected_element;
};

#endif
