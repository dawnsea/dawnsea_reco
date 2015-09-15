#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "seq.h"

#include "tree_space.h"


static int node_establish_tree(struct rb_root *root, long key)
{
	struct rb_node **new, *parent;
	struct item_node *this, *data;

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct item_node, node);

		parent = *new;
		
		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else {
			// dup
			return TRUE;
		}
	}
	
	// new
	data = malloc(sizeof(struct item_node));
	if (!data) {
		print_error("alloc error 1");
		return FALSE;
	}
	
	data->key 		= key;
	data->pref_tree = malloc(sizeof(struct rb_root));
	
	memset(data->pref_tree, 0, sizeof(struct rb_root));
	
	if (!data->pref_tree) {
		print_error("alloc error 2");
		free(data);
		return FALSE;
	}

	root->count++;
	
	print_debug(DEBUG_HARD, "count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);
		
	return TRUE;	
	
	
}

// 날데이터에서 아이템 스페이스의 트리를 구축한다. (트리 공간만 확보)
int make_item_space(struct rb_root *item_root, struct data_src_t *p) 
{
	while(p->item_key != ~0) {
		if (!node_establish_tree(item_root, p->item_key)) {
			printf("아이템 트리 셋업 실패");
			return FALSE;
		}
		p++;
	};
	
	return TRUE;	
}


// 날데이터에서 아이템과 사용자 스페이스의 트리를 구축한다.(트리 공간만 확보)
int make_space(struct rb_root *item_root, struct rb_root *user_root, struct data_src_t *p)
{
	while(p->item_key != ~0) {
		if (!node_establish_tree(user_root, p->user_key)) {
			printf("사용자 트리 셋업 실패");
			return FALSE;
		}
		
		if (!node_establish_tree(item_root, p->item_key)) {
			printf("아이템 트리 셋업 실패");
			return FALSE;
		}
				
		p++;
	};
	
	return TRUE;	
}
