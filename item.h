#ifndef __item_h__
#define __item_h__

#include "rbtree.h"
#include "node.h"

int item_insert_all(struct rb_root *root, struct data_src_t *p);
int all_insert_all(struct rb_root *item_root, struct rb_root *user_root, struct data_src_t *p);
int item_exist(struct rb_root *root, long key);
int item_all_erase(struct rb_root *root);


int item_all_insert(struct rb_root *root, struct data_src_t *p) ;
int item_insert(struct rb_root *root, long parent_key, long child_key, double pref);
int item_exist(struct rb_root *root, long key);

int reduce_item(struct rb_root *root, int coh);
struct data_src_t *reduce_raw_data(struct data_src_t *p, struct rb_root *item_root);
int conv_item(struct data_src_t *p, struct rb_root *user_root, struct rb_root *item_root);


int pref_insert(struct rb_root *root, long key, double pref);
int pref_erase(struct rb_root *root, long key);
void pref_erase_all(struct rb_root *root);


void dump_buf(struct data_src_t *p);

int node_all_erase(struct rb_root *root);
int item_all_dual_insert(struct rb_root *item_root, struct rb_root *user_root, struct data_src_t *p);

#endif
