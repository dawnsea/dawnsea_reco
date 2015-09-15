#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "rbtree_util.h"

#include "store_user.h"
#include "store_item.h"


static struct rb_root item_space_tree = RB_ROOT;
static struct rb_root *item_space_root = &item_space_tree;

static struct rb_root user_space_tree = RB_ROOT;
static struct rb_root *user_space_root = &user_space_tree;


struct rb_root *get_item_root(void)
{
	return item_space_root;
}

struct rb_root *get_user_root(void)
{
	return user_space_root;
}

#if 1 // 올드


int store_item(long item_key, long user_key, double pref)
{
	struct keeptalk_node *item_data;
	struct keeptalk_node *user_data;

	item_data = keeptalk_search(item_space_root, item_key);
	
	if(!item_data) {
		item_data = keeptalk_put_pref_tree(item_space_root, item_key);
		if (!item_data) {
			print_error("memory error 0");
			return FALSE;
		}
	}

	user_data = keeptalk_put_pref(item_data->val.user_pref_tree, user_key, pref);
	if (!user_data) {
		print_error("memory error 1");
		return FALSE;
	}
	
	user_data = keeptalk_search(user_space_root, user_key);
	
	if (!user_data) {
		user_data = keeptalk_put_pref_tree(user_space_root, user_key);
		if (!user_data) {
			print_error("memory error 2");
			return FALSE;
		}
	}
	
//	item_data = keeptalk_put_pref_tree(user_data->val.item_reco_tree, item_key);
	item_data = keeptalk_put_pref(user_data->val.item_pref_tree, item_key, pref);
	if (!item_data) {
		print_error("memory error 3");
		return FALSE;
	}	
	
	return TRUE;

}

#else 

int store_item(long item_key, long user_key, double pref)
{
	struct keeptalk_node *item_data;
	struct keeptalk_node *user_data;

	item_data = keeptalk_search(item_space_root, item_key);
	
	if(!item_data) {
		item_data = keeptalk_put_pref_tree(item_space_root, item_key);
		if (!item_data) {
			print_error("memory error 0");
			return FALSE;
		}
	}

	user_data = keeptalk_put_pref(item_data->val.user_pref_tree, user_key, pref);
	if (!user_data) {
		print_error("memory error 1");
		return FALSE;
	}
	
	return TRUE;
	
}

int store_user(long item_key, long user_key, double pref)
{
	struct keeptalk_node *item_data;
	struct keeptalk_node *user_data;
	
	user_data = keeptalk_search(user_space_root, user_key);
	
	if (!user_data) {
		user_data = keeptalk_put_pref_tree(user_space_root, user_key);
		if (!user_data) {
			print_error("memory error 2");
			return FALSE;
		}
	}
	
//	item_data = keeptalk_put_pref_tree(user_data->val.item_reco_tree, item_key);
	item_data = keeptalk_put_pref(user_data->val.item_pref_tree, item_key, pref);
	if (!item_data) {
		print_error("memory error 3");
		return FALSE;
	}	
	
	return TRUE;

}

#endif


long get_item_count(void)
{
	return get_node_count(item_space_root);
}

long get_user_count(void)
{
	return get_node_count(user_space_root);
}

