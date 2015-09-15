#define _GNU_SOURCE	1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <pthread.h>

#include "debug.h"

#include "rbtree.h"
#include "node.h"

#include "serial.h"
#include "reco_node.h"


#include "simi_euc.h"


//https://www.bionicspirit.com/blog/2012/01/16/cosine-similarity-euclidean-distance.html
// http://stats.stackexchange.com/questions/53068/euclidean-distance-score-and-similarity
inline double get_euc_simi(struct serial_data_t *this, struct serial_data_t *that)
{
	// this 아이템을 선호한 전체 사용자의 개별 선호값과
	// that 아이템을 선호한 전체 사용자의 개별 선호값을 이용하여 유클리디언 유사도를 구함
	
	double dot_sum = 0.0;
	double simi;
	double temp;
	
	while(1) {
		
		if (this->key != -1 && that->key != -1) {
			
			if (this->key == that->key) {
	
				temp = this->pref - that->pref;
				dot_sum += temp * temp;

				this++;
				that++;
			
			} else if (this->key > that->key) {
				dot_sum += that->pref * that->pref ;
				that++;
				
			} else {
				dot_sum += this->pref  * this->pref ;
				this++;
			}
						
			continue;
		}
		
		if (this->key == -1 && that->key == -1) {
			break;
		}
		
		if (this->key == -1) {			
			dot_sum += that->pref  * that->pref ;
			that++;
			continue;
		}
			
		dot_sum += this->pref  * this->pref ;
		this++;					
						
	}
		
	
#if 0	
	while (this->key != -1 || that->key != -1) {
		
		
		if (this->key != -1 && that->key != -1) {
		
		
			if (this->key == that->key) {
				
//				printf("this p = %lf, that p = %lf\n", this->pref, that->pref);
	
				temp = this->pref - that->pref;
				dot_sum += temp * temp;

				this++;
				that++;
			
			} else if (this->key > that->key) {
				dot_sum += that->pref * that->pref ;
				that++;
				
			} else {
				dot_sum += this->pref  * this->pref ;
				this++;
			}
		} else if (this->key == -1) {
			dot_sum += that->pref  * that->pref ;
			that++;
			
		} else {
			dot_sum += this->pref  * this->pref ;
			this++;			
			
		}


	
	}
#endif		

	if (dot_sum > 0.0) {

		simi = 1.0 / (1.0 + sqrt(dot_sum));
		if (simi == 1.0) {
			return 0.0;
		}
		return simi;
	}	

	return 0.0;
	
}


void *simi_euc(void *arg)
{

	struct simi_data_t my_data;
	struct simi_data_t *d = &my_data;

	struct serial_data_t *selected_item;
	struct serial_data_t *selected_user;
	
	long item_count = 0;
	double simi;
	
	memcpy((void *)d, arg, sizeof(struct simi_data_t));

	if (d->item_count < d->start) {
		print_error("calc end already, count = %ld, start = %ld", d->item_count, d->start);
		return NULL;
	}	
	
	item_count = d->start;

	print_info("start = %ld, len = %ld, count = %ld", d->start, d->len, d->item_count);

				
	// 아이템 배열을 순회한다.
	while(d->item_arr[item_count] && item_count < d->start + d->len && item_count < d->item_count) {
		
		// 현재 선택된 아이템의 사용자 배열을 가져온다.
		// 아래 값에는 사용자 키-선호도 쌍이 배열로 깔려있다.
		selected_item = d->item_arr[item_count];
		
//		print_info("item_count = %ld, dstart = %ld, dlen = %ld, ditemcount = %ld", item_count, d->start, d->len, d->item_count);
		
//		if (item_count % 50 == 0) print_info("prog = %ld", item_count);
	
		// 현재 아이템에 달려있는 사용자 배열을 순회한다.
		while(selected_item->key != -1) {
			//이 루프에서 키는 사용자 키다.
			
		
			// 사용자 키를 넣고 사용자 배열에서 아이템 배열을 가져옴
			selected_user = d->user_arr[selected_item->key];
			
			selected_user++; // 최초 아이템은 이미 골른 아이템과 같으므로 스킵한다.

			
			
			// 현재 사용자의 아이템 순회
			while(selected_user->key != -1) {
				// 이 루프에서 키는 아이템 키다

				// selected_item
				//		고정 : 선택된 아이템의 사용자 배열
				// 		순회 : 아이템 배열에서 아이템 키를 넣어서 사용자 배열을 가져온다.
				

				simi = get_euc_simi(selected_item, d->item_arr[selected_user->key]);
				
				if (simi > 0.0001) {
//					printf("item = %ld, user = %ld, iter = %ld, simi = %lf\n", item_count, selected_item->key, selected_user->key, simi);
				}
				
				
//				reco_insert(&reco_arr[selected_item->key], (long)simi << 32 | item_count, 100);
				reco_insert(&(d->reco_arr)[selected_item->key], (long)simi << 32 | selected_user->key, 100);
									
				
				selected_user++;
			}

			selected_item++;
		}
		item_count++;
	}

	print_info("thread out start = %ld", d->start);

	return NULL;
	
}

int multi_calc_simi_euc(int cpu, struct simi_data_t *args)
{

	long calc_pos;
	int thread_count;
	int cpu_max;
	int period;
	struct simi_data_t *cp, *cp_arg;
	
	pthread_t *count_thread;
	
	int index, result;  
	
	cpu_max = cpu == 0 ? (int)sysconf( _SC_NPROCESSORS_ONLN ) / 2 : cpu;
	
	thread_count = args->item_count / cpu_max;
	
	print_info("item count = %d", args->item_count);
	print_info("thread_count = %d", thread_count);
	print_info("cpu max = %d", cpu_max);
	
	count_thread = malloc(sizeof(pthread_t) * cpu_max);
	if (!count_thread) {
		print_error("thread alloc error 1");
		return FALSE;
	}
	
	memset(count_thread, 0, sizeof(pthread_t) * cpu_max);
	
	cp = malloc(sizeof(struct simi_data_t) * cpu_max);
	if (!cp) {
		print_error("thread alloc error 2");
		return FALSE;
	}	
	
	index = 0; 
	calc_pos = 0;
	period = 100;
	
	while(calc_pos < args->item_count) {	
		
		cp_arg = cp;
	
		for (index = 0; index < cpu_max; index++)  
		{  
			
			if (count_thread[index] == 0) {
				
				cp_arg->start 		= calc_pos;
				cp_arg->len   		= period;
				cp_arg->item_arr 	= args->item_arr;
				cp_arg->user_arr	= args->user_arr;	
				cp_arg->reco_arr	= args->reco_arr;
				cp_arg->item_count	= args->item_count;
				
				result = pthread_create(count_thread + index, NULL, simi_euc, (void *)cp_arg);  
				if (result < 0) {  
					print_error("thread create error!\n");  
				}  
				cp_arg++;
			
				calc_pos += period;
				print_info("thread = %d", index);
				
			}  
		}
		
//		print_info("thread out %d", index);
		
		for (index = 0; index < cpu_max; index++) {
			if (pthread_tryjoin_np(count_thread[index], NULL) == 0) {
				count_thread[index] = 0;
			} else {
				usleep(10000);
			}
		}	
	}
	
	for (index = 0; index < cpu_max; index++)  
		pthread_join(count_thread[index], NULL);  
	
	
	free(count_thread);
	free(cp);
	
	return TRUE;
}
