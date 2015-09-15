#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"
#include "pref.h"

#include "item.h"

// 새로 공간을 만들지는 않는다. 이미 만들어져 있다.
static int item_update(struct rb_root *root, long parent_key, long child_key, double pref)
{

	struct rb_node **new;
	struct item_node *this = NULL;
	int find_flag = FALSE;
	
	new = &(root->rb_node);
	
	while (*new) {
		
		this = container_of(*new, struct item_node, node);

		if (parent_key < this->key) {
			new = &((*new)->rb_left);
		} else if (parent_key > this->key) {
			new = &((*new)->rb_right);
		} else {
			// 찾으면 탈출
			find_flag = TRUE;
			break;
		}
	}
	
	
	if (!find_flag) {
		print_info("키가 없음");
		return FALSE;
	}

	if (!pref_insert(this->pref_tree, child_key, pref)) {
		print_info("차일드 인써트 실패");
		return FALSE;
	}

	return TRUE;
}

int item_insert_all(struct rb_root *root, struct data_src_t *p)
{
		
	while(p->item_key != ~0) {
		
//		print_info("i = %ld, u = %ld, p = %lf", p->item_key, p->user_key, p->pref);
		
		if (!item_update(root, p->item_key, p->user_key, p->pref)) {
			print_info("아이템 인써트 실패");
			return FALSE;
		}
		p++;
	};
	
	return TRUE;
}

int all_insert_all(struct rb_root *item_root, struct rb_root *user_root, struct data_src_t *p) 
{
		
	while(p->item_key != ~0) {
		
//		print_info("i = %ld, u = %ld, p = %lf", p->item_key, p->user_key, p->pref);
		if (!item_update(item_root, p->item_key, p->user_key, p->pref)) {
			print_info("아이템 인써트 실패");
			return FALSE;
		}
		
		if (!item_update(user_root, p->user_key, p->item_key, p->pref)) {
			print_info("아이템 인써트 실패");
			return FALSE;
		}
		p++;
		
	};
	
	return TRUE;
}


int item_erase(struct rb_root *root, long key)
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

int node_all_erase(struct rb_root *root)
{
	struct rb_node *p;
	struct item_node *this;
	
	p = rb_first(root);

	while(p) {
		this = container_of(p, struct item_node, node);
		
		pref_erase_all(this->pref_tree);
			
		root->count--;
		p = rb_next(p);

		rb_erase(&this->node, root);
		free(this);			
	}
	
	return TRUE;
}


// 아이템이 있는 아이템이냐?
int item_exist(struct rb_root *root, long key)
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
			return TRUE;
		}
	}
	return FALSE;
}



void dump_buf(struct data_src_t *p) 
{
	int i = 0;
	
	while(p->item_key != ~0) {
		
		print_info("u = %ld, i = %ld, p = %lf", p->user_key, p->item_key, p->pref);
		i++;
		
		if (i == 100) break;
		
		p++;
	};
	
}

