#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "serial.h"


struct serial_data_t **serial_node(struct rb_root *root)
{

	struct rb_node *p, *n;
	struct item_node *item_this;
	struct pref_node *pref_this;
	
	struct serial_data_t **item_arr, **iap, *pap;
	
	// 마지막 널 처리	
	item_arr = malloc(sizeof(struct serial_data_t *) * (root->count + 1));
	if (!item_arr) {
		print_info("alloc error!");
		return NULL;
	}
	iap = item_arr;
	
	p = rb_first(root);

	while(p) {
		item_this = container_of(p, struct item_node, node);
		
		// 마지막 널처리
		*iap = malloc(sizeof(struct serial_data_t) * (item_this->pref_tree->count + 1));
		if (!*iap) {
			print_info("alloc error2");
			exit(0); // 여기서 복구하기 어렵다. 앞에 다 free 쳐줘야 함.
		}
		
		n = rb_first(item_this->pref_tree);
		
		pap = *iap;
		
		while(n) {
			
			pref_this	= container_of(n, struct pref_node, node);
			pap->key 	= pref_this->key;
			pap->pref 	= pref_this->pref;
			
//			printf("parent key = %ld, child key = %ld, pref = %lf\n", item_this->key, pap->key, pap->pref);
			
			pap++;
			
			n = rb_next(n);
		}
		
		pap->key = -1;
//		memset((void *)pap, 0, sizeof(struct serial_data_t));
//		*(char *)pap = 0;
		
		iap++;
		p = rb_next(p);
	}
	
	iap = 0;

	return item_arr;

}

void dump_node(struct serial_data_t **data)
{
	long i = 0;
	struct serial_data_t *datum;

	while(*data) {
		
		datum = *data;
		while(datum->key != -1) {
			printf("parent key = %ld, child key = %ld, pref = %lf\n", i, datum->key, datum->pref);
			datum++;
		}
		data++;
		i++;
	}
	
}

