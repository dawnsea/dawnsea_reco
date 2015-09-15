#ifndef __debug_h__
#define __debug_h__

#define DEBUG_ALL		0			// 모두 출력하기
#define DEBUG_HARD		30			// 거의 출력하기
#define DEBUG_MID		20			// 가끔 출력하기
#define DEBUG_LOW		10			// 거의 드물게 출력하기

#define DEBUG_LEVEL_DEFAULT		DEBUG_ALL

//#define DEBUG 
//#define DEBUG_DEBUG		
//#define DEBUG_INFO
//#define DEBUG_WARN

#ifdef DEBUG
	
	
	#ifdef DEBUG_DEBUG
		#define print_debug(level, fmt, args...) do { \
				print_log(level, "\e[32m[debug]\e[m %s->line[%d] " fmt "\n", __FUNCTION__, __LINE__, ## args); \
			} while(0)
	#else
		#define print_debug(level, fmt, args...)
	#endif
	
	#ifdef DEBUG_INFO
		#define print_info(fmt, args...) do { \
				print_log(DEBUG_ALL, "\e[33m[info ]\e[m %s->line[%d] " fmt "\n", __FUNCTION__, __LINE__, ## args); \
			} while(0)
	#else
		#define print_info(fmt, args...)
	#endif	

	#ifdef DEBUG_WARN
		#define print_warn(fmt, args...) do { \
				print_log(DEBUG_ALL, "\e[34m[warn ]\e[m %s->line[%d] " fmt "\n", __FUNCTION__, __LINE__, ## args); \
			} while(0)
	#else
		#define print_warn(fmt, args...)
	#endif			
		
#else
	
	#define print_debug(level, fmt, args...)
	#define print_warn(fmt, args...)
	#define print_info(fmt, args...)
#endif
#define print_error(fmt, args...) do { \
		print_log(DEBUG_ALL, "\e[36m[error]\e[m %s->line[%d] " fmt "\n", __FUNCTION__, __LINE__, ## args); \
	} while(0)

	
void print_log(int level, const char *format, ...);
void init_log(FILE *fp);

void set_debug_level(int level);
int get_debug_level(void);

#endif
