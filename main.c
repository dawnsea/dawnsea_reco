#include <stdio.h>

#include "rbtree.h"
#include "test_all.h"

struct rb_root item_raw_tree 	= RB_ROOT;
struct rb_root *item_raw_root 	= &item_raw_tree;

struct rb_root user_raw_tree 	= RB_ROOT;
struct rb_root *user_raw_root 	= &user_raw_tree;


int main(int argc, char *argv[])
{
	test_all();

	return 0;
}
