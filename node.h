#ifndef __node_h__
#define __node_h__

#include "rbtree.h"

struct item_node {
	
	struct 	rb_node node;
	long	key;
	long 	seq;
	struct 	rb_root *pref_tree;
	
} __attribute__((aligned(sizeof(long))));

struct reco_node {
	
	struct 	rb_node node;
	long	key;
	
} __attribute__((aligned(sizeof(long))));


struct pref_node {
	
	struct 	rb_node node;
	long	key;
	double	pref;
	
} __attribute__((aligned(sizeof(long))));

struct data_src_t {
	long user_key;
	long item_key;
	double pref;
};


struct serial_data_t {
	long key;
	double pref;
};



#endif
