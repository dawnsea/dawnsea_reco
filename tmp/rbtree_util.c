#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "rbtree_util.h"


inline void tree_lock(volatile int *locked)
{
	while (__sync_val_compare_and_swap(locked, 0, 1));
    asm volatile("lfence" ::: "memory");
}

inline void tree_unlock(volatile int *locked)
{
    *locked = 0;
    asm volatile("sfence" ::: "memory");
}


#if 0
static struct rb_root item_tree = RB_ROOT;
static struct rb_root *item_root = &item_space_tree;

static struct rb_root user_tree = RB_ROOT;
static struct rb_root *user_root = &user_space_tree;
#endif



struct data_node *node_search(struct rb_root *root, long key);

int node_push(struct rb_root *root, long parent_key, long child_key, long pref);

int	node_erase(struct rb_root *root, long item_key);
void node_erase_all(struct rb_root *root);
int get_node_count(struct rb_root *root);






int get_node_count(struct rb_root *root)
{
	int count;
	
	tree_lock(&(root->locked));
	count = root->count;
	tree_unlock(&(root->locked));
	
	return count;

}

int node_update_pref(struct rb_root *root, long key, double pref)
{
	struct rb_node **new, *parent;
	struct data_node *this;
	
//	tree_lock(&(root->locked));

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct data_node, node);

		parent = *new;
		
		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else {
			// 이미 있다, update
			this->pref = pref;
//			tree_unlock(&(root->locked));
			return TRUE;
		}
	}
	
//	tree_unlock(&(root->locked));
	return FALSE;
}

int seq_insert(struct rb_root *root, long key)
{

	struct rb_node **new, *parent;
	struct seq_node *this, *data;

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct seq_node, node);

		parent = *new;
		
		if (key < this->key) {
			new = &((*new)->rb_left);
		} else if (key > this->key) {
			new = &((*new)->rb_right);
		} else {
			return TRUE;
		}
	}
	
	data = malloc(sizeof(struct seq_node));
	if (!data) {
		print_error("alloc error");
		return FALSE;
	}
	
	data->key = key;

	root->count++;
	
	print_debug(DEBUG_HARD, "count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);
		
	return TRUE;
}



int node_insert_pref(struct rb_root *root, long key, double pref)
{
	struct rb_node **new, *parent;
	struct data_node *this, *data;
	

	data = malloc(sizeof(struct data_node));
	if (!data) {
		print_error("alloc error");
		tree_unlock(&(root->locked));
		return FALSE;
	}
	
	data->key = key;
	data->val.pref = pref;
	
	tree_lock(&(root->locked));

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct data_node, node);

		parent = *new;
		
		if (data->key < this->key) {
			new = &((*new)->rb_left);
		} else if (data->key > this->key) {
			new = &((*new)->rb_right);
		} else {
			// 이미 있다, duplication
			free(data);
			this->pref = pref;
			tree_unlock(&(root->locked));
			return TRUE;
		}
	}

	root->count++;
	
	print_debug(DEBUG_HARD, "count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	this = container_of(&data->node, struct data_node, node);

	tree_unlock(&(root->locked));
		
	return TRUE;
}

// update가 없다. 
int node_insert_tree(struct rb_root *root, long key)
{
	struct rb_node **new, *parent;
	struct data_node *this, *data;
	
	data = malloc(sizeof(struct data_node));
	if (!data) {
		print_error("alloc error 1");
		return FALSE;
	}
	
	data->key = key;
	data->val.pref_tree = (struct rb_root *)malloc(sizeof(struct rb_root));
	if (!(data->val.pref_tree)) {
		print_error("alloc error 2");
		return FALSE;
	}
		
	memset(data->val.pref_tree, 0, sizeof(struct rb_root));
	
	
	tree_lock(&(root->locked));

	new = &(root->rb_node);
	parent = NULL;		
	
	while (*new) {
		
		this = container_of(*new, struct data_node, node);

		parent = *new;
		
		if (data->key < this->key) {
			new = &((*new)->rb_left);
		} else if (data->key > this->key) {
			new = &((*new)->rb_right);
		} else {
			// 이미 있다, duplication
			free(data);
			tree_unlock(&(root->locked));
			return TRUE;
		}
	}

	root->count++;
	
	print_debug(DEBUG_HARD, "count = %ld", root->count);

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	this = container_of(&data->node, struct data_node, node);

	tree_unlock(&(root->locked));
		
	return TRUE;
}


struct data_node *node_search(struct rb_root *root, long key)
{
	register struct rb_node *node;
	struct data_node *data;

	node = root->rb_node;

	while (node) {
		data = container_of(node, struct keeptalk_node, node);

		if (key < data->key) {
			node = node->rb_left;
		} else if (key > data->key) {
			node = node->rb_right;
		} else {
			return data;
		}
	}
	return NULL;
}

int node_push(struct rb_root *root, long parent_key, long child_key, long pref)
{

	
	
	
	
}



struct keeptalk_node *keeptalk_put_item(struct rb_root *root, long key, long item)
{
	struct keeptalk_node *data;
	
	data = malloc(sizeof(struct keeptalk_node));
	if (!data) {
		print_error("alloc error");
		return NULL;
	}
	
	data->key = key;
	data->val.ival = item;
	
	return keeptalk_insert(root, data);
}

struct keeptalk_node *keeptalk_put_pref(struct rb_root *root, long key, double pref)
{
	struct keeptalk_node *data;
	
	data = malloc(sizeof(struct keeptalk_node));
	if (!data) {
		print_error("alloc error");
		return NULL;
	}
	
	data->key = key;
	data->val.pref = pref;
	
	return keeptalk_insert(root, data);
}

struct keeptalk_node *keeptalk_put_reco(struct rb_root *root, long key, long reco_item)
{
	struct keeptalk_node *data;
	
	data = malloc(sizeof(struct keeptalk_node));
	if (!data) {
		print_error("alloc error");
		return NULL;
	}

	data->key = key;
	data->val.reco_item = reco_item;
	
	return keeptalk_insert(root, data);
}

struct keeptalk_node *keeptalk_put_pref_tree(struct rb_root *root, long key)
{
	struct keeptalk_node *data;
	
	data = malloc(sizeof(struct keeptalk_node));
	if (!data) {
		print_error("alloc error 1");
		return NULL;
	}
	
	data->key = key;
	
	data->val.user_pref_tree = (struct rb_root *)malloc(sizeof(struct rb_root));
	if (!(data->val.user_pref_tree)) {
		print_error("alloc error 2");
		return NULL;
	}
		
	memset(data->val.user_pref_tree, 0, sizeof(struct rb_root));
//	성능을 위하여 메모리 작업은 그냥 따로 한다.

	data->opt.item_reco_tree = (struct rb_root *)malloc(sizeof(struct rb_root));
	if (!(data->opt.item_reco_tree)) {
		print_error("alloc error 2");
		return NULL;
	}
		
	memset(data->opt.item_reco_tree, 0, sizeof(struct rb_root));
	
	
	return keeptalk_insert(root, data);
}

int keeptalk_erase(struct rb_root *root, long key)
{

	struct keeptalk_node *data;
	
	tree_lock(&(root->locked));
		
	data = keeptalk_search(root, key);

	if (data) {
		
		root->count--;
		
		rb_erase(&data->node, root);
        free(data);
        
	} else {
		
		tree_unlock(&(root->locked));
		return FALSE;
	}
	
	tree_unlock(&(root->locked));	
	return TRUE;
}

void keeptalk_erase_all(struct rb_root *root)
{
	struct rb_node *n;
	struct keeptalk_node *this;
	
	tree_lock(&(root->locked));
	
	n = rb_first(root);

	while (n) {
		
		this = container_of(n, struct keeptalk_node, node);
		n = rb_next(n);
		
		rb_erase(&this->node, root);
		free(this);
	}
	
	root->count = 0;
	
	tree_unlock(&(root->locked));
}

void test_keeptalk_rbtree(void)
{
	
	struct rb_root test_tree = RB_ROOT;
	struct rb_root *test_root = &test_tree;
	struct keeptalk_node *data;
	
	long i;
	
	printf("user space rb tree test\n");
	printf("1 : input data\n");
	
	for (i = 0; i < 100; i++) {
		keeptalk_put_item(test_root, i, i * 2);
	}
	
	printf("count = %ld\n", test_root->count);
	
	printf("2 : search data\n");
	
	for (i = 0; i < 100; i++) {
		data = keeptalk_search(test_root, i);
		printf("key = %ld, val = %ld\n", data->key, data->val.ival);
	}	
	
	printf("3 : [2] item delete\n");
	
	keeptalk_erase(test_root, 2);
	
	for (i = 0; i < 100; i++) {
		data = keeptalk_search(test_root, i);
		if (data) 
			printf("key = %ld, val = %ld\n", data->key, data->val.ival);
	}		
	
	printf("count = %ld\n", test_root->count);
	
	printf("4 : all item delete\n");
	keeptalk_erase_all(test_root);

	for (i = 0; i < 100; i++) {
		data = keeptalk_search(test_root, i);
		if (data)
			printf("key = %ld, val = %ld\n", data->key, data->val.ival);
	}			
	
	printf("count = %ld\n", test_root->count);
	
	printf("test end\n");

}
