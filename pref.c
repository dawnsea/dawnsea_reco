#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "pref.h"

int pref_insert(struct rb_root *root, long key, double pref) 
{
		
	struct rb_node **new, *parent;
	struct pref_node *this, *data;

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct pref_node, node);

		parent = *new;
		
		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else if (key == this->key) {
			return TRUE;
		} 
	}
	
	data = malloc(sizeof(struct pref_node));
	if (!data) {
		print_error("alloc error");
		return FALSE;
	}

	data->key 	= key;
	data->pref 	= pref;

	root->count++;
	
	print_debug(DEBUG_HARD, "count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);
		
	return TRUE;
}


int pref_erase(struct rb_root *root, long key)
{

	struct rb_node **new;
	struct pref_node *this;

	new = &(root->rb_node);
	
	while (*new) {
		
		this = container_of(*new, struct pref_node, node);

		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else {
			// 찾았다.
			root->count--;
			rb_erase(&this->node, root);
			free(this);
			
			return TRUE;
		}
	}
	
	print_info("삭제 중 못 찾음");	
	return FALSE;
}

void pref_erase_all(struct rb_root *root)
{
	struct rb_node *n;
	struct pref_node *this;
	
	n = rb_first(root);

	while (n) {
		
		this = container_of(n, struct pref_node, node);
		n = rb_next(n);
		rb_erase(&this->node, root);
		free(this);
	}
	
	root->count = 0;

}
