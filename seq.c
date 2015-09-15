#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "seq.h"


int seq_all_seq(struct rb_root *root)
{
	struct rb_node *p;
	struct item_node *this;
	long seq = 0;
	
	p = rb_first(root);
	
	while(p) {
		this = container_of(p, struct item_node, node);
		this->seq = seq++;
//		print_info("key = %ld, seq = %ld", this->key, this->seq);
		p = rb_next(p);
	}
	
	return TRUE;
}

static long get_seq(struct rb_root *root, long key)
{
		
	struct rb_node **new;
	struct item_node *this;

	new = &(root->rb_node);
	
	while (*new) {
		
		this = container_of(*new, struct item_node, node);
		
		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else {
			return this->seq;
		}
	}
	
	return -1;
	
}

int conv_raw_data(struct data_src_t *p, struct rb_root *user_root, struct rb_root *item_root) 
{
	long user_key;
	long item_key;
	
	while(p->item_key != ~0) {

		user_key = get_seq(user_root, p->user_key);
		item_key = get_seq(item_root, p->item_key);
		
		if (user_key == -1 || item_key == -1) {
			print_info("변환 에러");
			return FALSE;
			
		} 
		
		p->user_key = user_key;
		p->item_key = item_key;
		
		p++;
	};
	
	return TRUE;	
	
}
