#ifndef __tree_space_h__
#define __tree_space_h__

#include "rbtree.h"
#include "node.h"

int make_item_space(struct rb_root *item_root, struct data_src_t *p);
int make_space(struct rb_root *item_root, struct rb_root *user_root, struct data_src_t *p);

#endif

