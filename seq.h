#ifndef __seq_h__
#define __seq_h__

#include "rbtree.h"
#include "node.h"

int seq_all_seq(struct rb_root *root);
int conv_raw_data(struct data_src_t *p, struct rb_root *user_root, struct rb_root *item_root);

#endif
