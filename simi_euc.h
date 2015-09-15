#ifndef __simi_euc_h__
#define __simi_euc_h__

#include "rbtree.h"
#include "node.h"

struct simi_data_t {
	
	int start;
	int len;

	struct serial_data_t **item_arr;
	struct serial_data_t **user_arr;
	
	long item_count;
	
	struct rb_root *reco_arr;
	
};



void *simi_euc(void *arg);
int multi_calc_simi_euc(int cpu, struct simi_data_t *args);

#endif
