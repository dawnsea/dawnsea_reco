#ifndef __serial_h__
#define __serial_h__

#include "rbtree.h"
#include "node.h"

struct serial_data_t **serial_node(struct rb_root *root);
void dump_node(struct serial_data_t **data);

#endif
