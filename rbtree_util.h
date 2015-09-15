#ifndef __rbtree_util_h__
#define __rbtree_util_h__

#include "rbtree.h"


struct seq_node {
	
	struct 	rb_node node;
	long	key;
	long	seq;
	
} __attribute__((aligned(sizeof(long))));

struct data_node {
	
	struct 	rb_node node;
	long	key;
	long	seq;
	
	union {
		double	pref;
		struct 	rb_root *pref_tree;		
	} val;
	
} __attribute__((aligned(sizeof(long))));

struct data_node *node_search(struct rb_root *root, long key);

int node_push(struct rb_root *root, long parent_key, long child_key, long pref);

int	node_erase(struct rb_root *root, long item_key);
void node_erase_all(struct rb_root *root);
int get_node_count(struct rb_root *root);


void test_keeptalk_rbtree(void);


#endif
