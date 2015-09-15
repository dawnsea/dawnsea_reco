#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "reco_node.h"


struct rb_root *make_reco_tree(struct rb_root *root)
{
	struct rb_root *reco_arr;
	
	// 마지막 널 처리	
	reco_arr = malloc(sizeof(struct rb_root) * (root->count + 1));
	if (!reco_arr) {
		print_info("alloc error!");
		return NULL;
	}
	
	memset(reco_arr, 0, sizeof(struct rb_root) * (root->count + 1));
	
	return reco_arr;
}


int reco_insert(struct rb_root *root, long simi_and_key, int limit) 
{
		
	struct rb_node **new, *parent;
	struct reco_node *this, *data;

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct reco_node, node);

		parent = *new;
		
		if (simi_and_key < this->key) {
			new = &((*new)->rb_left);
		} else if (simi_and_key > this->key) {
			new = &((*new)->rb_right);
		} else if (simi_and_key == this->key) {
			return TRUE;
		} 
	}
	
	data = malloc(sizeof(struct reco_node));
	if (!data) {
		print_error("alloc error");
		return FALSE;
	}

	data->key 	= simi_and_key;
	root->count++;
	
	print_debug(DEBUG_HARD, "count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);
	
	if (root->count > limit) {


		struct rb_node *remove;

		remove = rb_last(root);
		
		root->count--;
		rb_erase(remove, root);
		free(remove);
	}	
		
	return TRUE;
}

void dump_reco(struct rb_root *reco_arr, long count) 
{
	int i;
	
	for (i = 0; i < count; i++) {
		print_info("count = %ld", reco_arr[i].count);
	}	
}

