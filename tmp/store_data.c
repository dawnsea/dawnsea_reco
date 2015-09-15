#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"
#include "input_file.h"
#include "debug.h"

#include "store_data.h"
#include "store_item.h"
#include "store_user.h"

#define NUM_LEN_MAX				10

int load_data(const char *filename)
{
	char *p;
	int fd;
	
	char num_buf[NUM_LEN_MAX];
	char *np;
	
	long			user_key 		= 0;
	long 			item_key		= 0;
	double			pref			= 0.0;
	long 			count			= 0;
	int				ret;
	
	
	fd = attach_data(filename, &p);
	print_info("attach ok %p", p);
	
	np = (char *)num_buf;
	
	while(*p) {
		
		if (*p != '\t' && *p != '\n') {
			*np++ = *p++;
			continue;
		} 
		
		p++;
		
		// 분리
		*np = 0;
		np = num_buf;			
		
//		printf("num_buf = %s", np);

		switch (count) {
			case 0:
				user_key = atol(np);
				count++;
				break;
			case 1:
				count++;
				break;
				
			case 2:
				item_key = atol(np);
				count++;
				break;
				
			case 3:
				pref = ((atof(np) + 500.0)/ 1000.0);
				
//				printf("item = %d, user = %d, pref = %f", item_key, user_key, pref);
				
				ret = store_item(item_key, user_key, pref);
				if (!ret) {
					print_error("item insert error\n");
					count = 0;
					return FALSE;
				}
								
				count = 0;
				break;
			
			default: // 에러
				return FALSE;
		}
		
	}
	detach_data(fd, p);
	
	print_debug(DEBUG_ALL, "data load ok!");

	return TRUE;
}
