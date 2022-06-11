#ifndef H_MH
#define H_MH

#include "def.h"
#include "short.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// can be skipped if no auto-malloc was done
void mh_free(struct mh *mh);

// auto-mallocs the needed buffers if given NULL
// pointers and MH_STATIC is defined
bool mh_init(struct mh *mh, void *buf_branches, uint16_t len_branches,
             uint16_t alloc_step_len_branches,
             uint16_t alloc_maximum_len_brances, void *buf_elements,
             uint16_t len_elements, uint16_t alloc_step_len_elements,
             uint16_t alloc_maximum_len_elementsm, uint16_t sizeof_element,
             void *buf_holes, uint16_t len_holes, uint16_t alloc_step_len_holes,
             uint16_t alloc_maximum_len_holes);

bool mh_branch_exec(struct mh *mh,
                    bool (*function)(struct mh *mh, struct mh_branch *branch,
                                     void *ptr),
                    void *ptr);

void mh_defrag(struct mh *mh);

void mh_add_element(struct mh *mh);

void mh_rm_branch(struct mh *mh);

void mh_rm_element(struct mh *mh);

uint32_t mh_count_branches(struct mh *mh);

#endif
