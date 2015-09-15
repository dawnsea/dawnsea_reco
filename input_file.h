#ifndef __input_file_h__
#define __input_file_h__


#include "node.h"

long get_data_rows(char *p);

struct data_src_t *load_data(char *filename);

#endif
