#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "pref.h"
#include "item.h"

#include "limit.h"


// cooccurrence
int reduce_item(struct rb_root *root, int limit)
{
	struct rb_node *p, *n;
	struct item_node *this;
	
	p = rb_first(root);

	while(p) {
		this = container_of(p, struct item_node, node);
		
		if (this->pref_tree->count <= limit) {
//			print_info("아이템 밑의 전체 사용자 트리 삭제");
			pref_erase_all(this->pref_tree);
			
//			print_info("현재 아이템 삭제");
			root->count--;
			n = p;
			p = rb_next(p);
			
			this = container_of(n, struct item_node, node);
			
			rb_erase(&this->node, root);
			free(this);			
			continue;
		}
		p = rb_next(p);
	}
	
	return TRUE;
}

struct data_src_t *reduce_raw_data(struct data_src_t *p, struct rb_root *item_root) 
{
	struct data_src_t *new_p, *tp, *ntp;
	
	tp = p;
	
	long count = 0;

	print_info("제거된 아이템 마킹 시작");
	while(tp->item_key != ~0) {
		if (item_exist(item_root, tp->item_key)) {
			count++;
		} else {
//			print_info("못 찾음");
			tp->user_key = -1; 		// 삭제된 아이템은 마킹함.
		}
		tp++;
	};
	
	print_info("제거된 아이템 마킹 완료");
	print_info("전체 데이터 열 수 = %ld", count);
	
	new_p = malloc((count + 1) * sizeof(struct data_src_t));  // 한 칸 더 많이. 마지막 끝으로 셋팅하기 위함

	tp = p;
	ntp = new_p;

	while(tp->item_key != ~0) {
		if (tp->user_key != -1) {	// 마킹되어 있으면
			ntp->user_key 	= tp->user_key;
			ntp->item_key 	= tp->item_key;
			ntp->pref		= tp->pref;
			ntp++;
		} 
		tp++;
	};		

	ntp->item_key = ~0;

	return new_p;	
	
}


