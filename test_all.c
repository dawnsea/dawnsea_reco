#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "input_file.h"
#include "profile.h"
#include "seq.h"
#include "item.h"
#include "tree_space.h"
#include "limit.h"
#include "serial.h"
#include "simi_euc.h"
#include "reco_node.h"


extern struct rb_root *item_raw_root;
extern struct rb_root *user_raw_root;


static void test_debug(void)
{
	init_log(stderr);
	set_debug_level(DEBUG_LOW);
	print_debug(DEBUG_ALL, "bug = %s", "shit");
	print_warn("bug = %s", "shit");
	print_info("bug = %s", "shit");
	print_error("bug = %s", "shit");
}
 
static void test_data_input(void)
{
	long st, et, sst;
	
	struct data_src_t *dp, *new_dp;
	
	st = get_usec();
	sst = st;
	
	print_info("01. 데이터 로드 시작 >>>>>");
	dp = load_data("./r.tsv");
	
	if (!dp) {
		print_info("mem alloc error");
		return;
	}
	
	et = get_usec();
	print_info("02. 데이터 완료 = %ld ms", (et - st) / 1000);
	

	
	
	
	
	st = et;	
	print_info("03. 아이템 트리 공간 구축");
	
	if (!make_item_space(item_raw_root, dp)) {
		print_info("아이템 트리 공간 구축 실패");
		return;
	}

	et = get_usec();
	print_info("03. 아이템 트리 공간 구축 완료 = %ld ms", (et - st) / 1000);	




		
	st = et;	
	print_info("04. 아이템 트리 전체 입력");	
	
	if (!item_insert_all(item_raw_root, dp)) {
		print_info("아이템 트리 전체 입력 실패");
		return;;
	}
	
	et = get_usec();
	print_info("04. 아이템 트리 전체 입력 완료 = %ld ms", (et - st) / 1000);
	print_info("05. 아이템 총 개수 = %ld", item_raw_root->count);
	
	
#define LIMIT_COOCUR	100


	st = et;	
	print_info("06. 아이템 줄이기 (%d) 명 이상) 시작", LIMIT_COOCUR, item_raw_root->count);
	reduce_item(item_raw_root, LIMIT_COOCUR);
	
	et = get_usec();
	print_info("06. 아이템 줄이기 완료 = %ld ms ", (et - st) / 1000);
	print_info("07. 현재 아이템 총 개수 = %ld", item_raw_root->count);
	
	
	
	print_info("08. 원본 데이터에 제거 반영");
	new_dp = reduce_raw_data(dp, item_raw_root);
	free(dp);
	print_info("08. 원본 데이터에 제거 반영 완료");
	

//	dump_buf(new_dp);

	print_info("09. 최초 트리 삭제");
	node_all_erase(item_raw_root);
	print_info("09. 최초 트리 삭제 완료");
	
	st = et;	
	print_info("10. 아이템, 사용자 트리 전체 입력");	
	if (!make_space(item_raw_root, user_raw_root, new_dp)) {
		print_info("사용자 구축 실패");
		return;
	}	
	
	if (!all_insert_all(item_raw_root, user_raw_root, new_dp)) {
		print_info("아이템, 사용자 전체 입력 실패");
		return;;
	}
	
	et = get_usec();
	print_info("10. 아이템, 사용자 트리 전체 입력 완료 = %ld ms", (et - st) / 1000);
	print_info("10. 아이템 총 수 = %ld", item_raw_root->count);
	print_info("10. 사용자 총 수 = %ld", user_raw_root->count);
	

	print_info("11. 아이템 키, 사용자 키 시퀀스 추가 >>>>>");
	seq_all_seq(item_raw_root);
	seq_all_seq(user_raw_root);
	print_info("11. 아이템 키, 사용자 키 시퀀스 추가 완료 >>>>>");
	
	print_info("12. 시퀀스 번호로 바꾸기");
	
	if(!conv_raw_data(new_dp, user_raw_root, item_raw_root)){
		print_info("키변환 에러");	
	}
	print_info("12. 시퀀스 번호로 바꾸기 완료");
		
//	dump_buf(new_dp);
	
	print_info("13. 최초 트리 삭제>>>>>");
	node_all_erase(item_raw_root);
	node_all_erase(user_raw_root);
	print_info("13. 최초 트리 삭제 끝>>>>>");	
	
	if (!make_space(item_raw_root, user_raw_root, new_dp)) {
		print_info("사용자 구축 실패");
		return;
	}	
	
	st = et;	
	print_info("20. 아이템, 사용자 트리 전체 입력");	
	
	if (!all_insert_all(item_raw_root, user_raw_root, new_dp)) {
		print_info("아이템, 사용자 전체 입력 실패");
		return;;
	}
	
	et = get_usec();
	print_info("20. 아이템, 사용자 트리 전체 입력 완료 = %ld ms", (et - st) / 1000);
	print_info("20. 아이템 총 수 = %ld", item_raw_root->count);
	print_info("20. 사용자 총 수 = %ld", user_raw_root->count);
	
	et = get_usec();
	
	
	free(new_dp);
	
	st = et;
	
	print_info("21. 데이터 직렬화");	
	struct serial_data_t **item_arr;
	struct serial_data_t **user_arr;
	
	printf("아이템 직렬화\n");
	item_arr = serial_node(item_raw_root);
	
	printf("사용자 직렬화\n");
	user_arr = serial_node(user_raw_root);
	et = get_usec();
	print_info("21. 데이터 직렬화 끝 = %ld ms", (et - st) / 1000);
	
	print_info("key = %ld, pref = %lf", item_arr[10]->key, item_arr[10]->pref);
	print_info("key = %ld, pref = %lf", user_arr[10]->key, user_arr[10]->pref);
	
	print_info("소요시간 = %ld ms", (et - sst) / 1000);
	

//	printf("아이템 덤프\n");
//	dump_node(item_arr);
//	printf("사용자 덤프\n");
//	dump_node(user_arr);
	
//	return;	

	struct rb_root *reco_arr;
	reco_arr = make_reco_tree(user_raw_root);
	
	struct simi_data_t arg;
	arg.item_arr 	= item_arr;
	arg.user_arr 	= user_arr;
	arg.reco_arr 	= reco_arr;
	arg.item_count 	= item_raw_root->count;
	
	multi_calc_simi_euc(8, &arg);
	
//	simi_euc((void *)&arg);
	
	et = get_usec();
	print_info("소요시간 = %ld ms", (et - sst) / 1000);
	
//	dump_reco(reco_arr, user_raw_root->count);

	return;

}
 
void test_all(void)
{

	test_debug();
	test_data_input();	
	
	
}



#if 0 

//gcc -o test pre_calc.c simi_euc.c rbtree.c rbtree_util.c main.c test_all.c input_file.c debug.c store_data.c store_item.c multi_loader.c -lm -lpthread -DDEBUG -DDEBUG_INFO -DDEBUG_DEBUG -Wall
// gcc -o test rbtree.c rbtree_util.c main.c test_all.c input_file.c debug.c store_data.c store_item.c multi_loader.c -lm -lpthread -DDEBUG -DDEBUG_INFO -DDEBUG_DEBUG -Wall 
// gcc -o test rbtree.c rbtree_util.c main.c test_all.c input_file.c debug.c store_data.c store_item.c multi_loader.c -lm -lpthread -DDEBUG -DDEBUG_INFO -DDEBUG_DEBUG -Wall 
#include <stdio.h>
#include <stdint.h>

#include <linux/kernel.h>
#include <linux/stddef.h>

#include "rbtree_util.h"
#include "input_file.h"

#include "store_data.h"
#include "store_item.h"
#include "multi_loader.h"
#include "store_user.h"
#include "simi_cos.h"
#include "pre_calc.h"

static void test_data_input(void)
{
	char *p;
	int fd;
	fd = attach_data("./r.tsv", &p);
	
	printf("%c%c\n", *p,*(p+1));
	
	detach_data(fd, p);
}

static void test_typeof(void)
{
	int *p;
	
	typeof(p) a;
	printf("type = %ld\n", sizeof(a));
	
	printf("int = %ld\n", sizeof(int));
	printf("int * = %ld\n", sizeof(int *));
	printf("uint64_t = %ld\n", sizeof(uint64_t));
}



static void test_store_data(void)
{
//	load_data("./round2_purchaseRecord.tsv");
	load_data("./r.tsv");
#if 0
	load_data("/troot/bd/analy/dawnsea_test/000000_0");
	load_data("/troot/bd/analy/dawnsea_test/000001_0");
	load_data("/troot/bd/analy/dawnsea_test/000002_0");
	load_data("/troot/bd/analy/dawnsea_test/000003_0");
	load_data("/troot/bd/analy/dawnsea_test/000004_0");
	load_data("/troot/bd/analy/dawnsea_test/000005_0");
	load_data("/troot/bd/analy/dawnsea_test/000006_0");
	load_data("/troot/bd/analy/dawnsea_test/000007_0");
	load_data("/troot/bd/analy/dawnsea_test/000008_0");
	load_data("/troot/bd/analy/dawnsea_test/000009_0");
	load_data("/troot/bd/analy/dawnsea_test/000010_0");
	load_data("/troot/bd/analy/dawnsea_test/000011_0");
	load_data("/troot/bd/analy/dawnsea_test/000012_0");
	load_data("/troot/bd/analy/dawnsea_test/000013_0");
#endif	
#if 0
	const char *files[] = { 	"/troot/bd/analy/dawnsea_test/000000_0", 
								"/troot/bd/analy/dawnsea_test/000001_0", 
								"/troot/bd/analy/dawnsea_test/000002_0", 
								"/troot/bd/analy/dawnsea_test/000003_0", 
								"/troot/bd/analy/dawnsea_test/000004_0", 
								"/troot/bd/analy/dawnsea_test/000005_0", 
								"/troot/bd/analy/dawnsea_test/000006_0", 
								"/troot/bd/analy/dawnsea_test/000007_0", 
								"/troot/bd/analy/dawnsea_test/000008_0", 
								"/troot/bd/analy/dawnsea_test/000009_0", 
								"/troot/bd/analy/dawnsea_test/000010_0", 
								"/troot/bd/analy/dawnsea_test/000011_0",
								"/troot/bd/analy/dawnsea_test/000012_0",
//								"/troot/bd/analy/dawnsea_test/000013_0", 					
								NULL};
	multi_loader(files);

	const char *files[] = { 	
//								"/troot/bd/analy/dawnsea_test/000000_0",
								"/troot/bd/analy/dawnsea_test/000001_0",
//								"/troot/bd/analy/dawnsea_test/000002_0",
//								"/troot/bd/analy/dawnsea_test/000003_0",
								NULL};								
	multi_loader(files);
	
#endif	
}


void test_all(void)
{
//	test_typeof();
//	test_data_input();
	test_debug();
//	test_keeptalk_rbtree();

	test_store_data();
	print_info("item count = %ld", get_item_count());
	print_info("pref calc start");
	summary_item_space(0.0, 100);
	print_info("pref calc end");
	print_info("item count = %ld", get_item_count());
	print_info("user count = %ld", get_user_count());
	conv_item();
//	test_user();
//	conv_user();
//	dump_item();
	print_info("아이템 유사도 구하기 시작");
//	multi_calc_simi_cos(8);
	multi_calc_simi_euc(8);
//	get_user_reco(3705754);

//	dump_reco();
	
//	get_users_reco_cos();

	print_info("저장");
	get_users_reco();



//	multi_calc_pref();
//	print_info("user max = %ld", get_user_max());
	

}




#endif
