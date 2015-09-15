#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

static FILE *log_output = NULL; 

static int debug_level = DEBUG_LEVEL_DEFAULT;

void init_log(FILE *fp)
{
	log_output = !fp ? stdout : fp;
	
}

void set_debug_level(int level)
{
	debug_level = level;
}

int get_debug_level(void)
{
	return debug_level;
}

void print_log(int level, const char *format, ...)
{
    va_list ap;
    
    if (level <= debug_level) {

		va_start(ap, format);
		vfprintf(log_output, format, ap);
		va_end(ap);
	}
}
