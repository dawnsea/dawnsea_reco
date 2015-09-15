#include <time.h>
#include <sys/time.h>
 
#include "profile.h"

long get_usec(void) 
{
	struct timeval tv;
	
	gettimeofday(&tv,NULL);
	
	return (1000000 * tv.tv_sec + tv.tv_usec);
}
	
