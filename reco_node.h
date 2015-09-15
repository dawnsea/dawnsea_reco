#ifndef __reco_node_h__
#define __reco_node_h__

struct rb_root *make_reco_tree(struct rb_root *root);
int reco_insert(struct rb_root *root, long simi_and_key, int limit);
void dump_reco(struct rb_root *reco_arr, long count);

#endif
