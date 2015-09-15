#ifndef __limit_h__
#define __limit_h__

#include "rbtree.h"
#include "node.h"

int reduce_item(struct rb_root *root, int limit);
struct data_src_t *reduce_raw_data(struct data_src_t *p, struct rb_root *item_root);


#endif
