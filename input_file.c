#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HDFS_SUPPORT
#include "hdfs.h"
#endif

#include "debug.h"

#include "input_file.h"

#ifndef FALSE
	#define TRUE		1
	#define FALSE		0
#endif

static int attach_data(const char *file_name, char **addr)
{
	struct stat sb;
	int fd;
	
    fd = open(file_name, O_RDONLY);
    
    if (fd == -1) {
		print_error("File open error");
		exit(EXIT_FAILURE);
	}
    
    if (fstat(fd, &sb) == -1) {
		
		print_error("File size get error");
		
		close(fd);
        exit(EXIT_FAILURE);
    }
        
    *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (*addr == MAP_FAILED) {
		
		print_error("MMAP fail");
		
		close(fd);
		exit(EXIT_FAILURE);
	}
    
    return fd;
}

static void detach_data(int fd, char *addr)
{
	struct stat sb;

    if (fstat(fd, &sb) == -1) {
		print_error("File size get error");
        exit(EXIT_FAILURE);
    }
	
	munmap(addr, sb.st_size);
	close(fd);
}

long get_data_rows(char *p)
{
	long count = 0;
	
	while(*p) {
		if (*p == '\n') count++;
		p++;
	};
	
	print_info("rows = %ld", count);
	
	return count;
	
}

#define NUM_LEN_MAX		255
struct data_src_t *load_data(char *filename)
{
	int fd;
	char num_buf[NUM_LEN_MAX];
	char *np, *p;
	long count, rows;
	
	struct data_src_t *dp, *cdp;	

#ifdef HDFS_SUPPORT

	hdfsFS fs;
    hdfsFileInfo *fi;
    hdfsFile rfile;
    
//    const char* workpath = "hdfs://skpds/user/tadsvc/dawnsea/hdfstest";
	int fnum, i, rsize, tsize;
	char *addr, *p;


	fd = open("./dawnsea_temp", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	if (fd == -1) {
		printf("temp open error, %s\n", strerror(errno));
		exit(-1);
	}

	fs = hdfsConnect("default", 0);
	fi = hdfsListDirectory(fs, filename, &fnum);

	tsize = 0;

	
	for (i = 0; i < fnum; i++) {

		printf("%02d:%s, %ld\n", i, (fi + i)->mName, (fi + i)->mSize);
		rfile = hdfsOpenFile(fs, (fi + i)->mName, O_RDONLY, 0, 0, 0);
		tsize += (fi + i)->mSize;

		hdfsCloseFile(fs, rfile);

	}

	if (lseek(fd, tsize-1, SEEK_SET) == -1) {
		close(fd);
		perror("Error calling lseek() to 'stretch' the file");
		exit(EXIT_FAILURE);
	}
	if (write(fd, "", 1) == -1) {
		close(fd);
		perror("Error writing last byte of the file");
		exit(EXIT_FAILURE);
	}

	addr = mmap(0, tsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		printf("mmap error!, %s", strerror(errno));
		exit(0);
	}

	p = addr;

	for (i = 0; i < fnum; i++) {

		printf("%02d:%s, %ld\n", i, (fi + i)->mName, (fi + i)->mSize);
		rfile = hdfsOpenFile(fs, (fi + i)->mName, O_RDONLY, 0, 0, 0);

		rsize = 4096;

		for (; rsize == 4096;) {
			rsize = hdfsRead(fs, rfile, (void *)temp, rsize);
			memcpy(p, temp, rsize);
			p += rsize;

		}
		hdfsCloseFile(fs, rfile);
	}

	if (msync(addr, tsize, MS_SYNC) == -1) {
		printf("msync error, %s\n", strerror(errno));
		exit(0);
	}

	hdfsDisconnect(fs);

#else
	
	
	fd = attach_data(filename, &p);
#endif
	
	print_info("attach ok %p", p);
	
	rows = get_data_rows(p);
	print_info("rows = %ld", rows);
	
	dp = malloc((rows + 1) * sizeof(struct data_src_t));  // 한 칸 더 많이. 마지막 끝으로 셋팅하기 위함
	if (!dp) {
		print_info("data load fail");
		detach_data(fd, p);
		return NULL;
	}
	
	print_info("mem alloc ok");
	
	cdp = dp;
	
	
	np = (char *)num_buf;
	
	count = 0;
		
	while(*p) {
		
		if (*p != ',' && *p != '\n') {
//		if (*p != '\t' && *p != '\n') {
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
				cdp->user_key = atol(np);
				count++;
				break;
				
//			case 1:
//				count++;
//				break;
				
			case 1:
				cdp->item_key = atol(np);
				count++;
				break;
				
			case 2:
//				cdp->pref = ((atof(np) + 500.0)/ 1000.0);				
				cdp->pref = atof(np) + 0.1;
//				printf("item = %d, user = %d, pref = %f", item_key, user_key, pref);	
				cdp++;
				count = 0;
				break;
			
			default: // 에러
				return FALSE;
		}
		
	}
	
	cdp->item_key = ~0;
	
	print_info("last = %lx", cdp->item_key);
	
	detach_data(fd, p);
	
	print_debug(DEBUG_ALL, "data load ok!");	
	
	return dp;
	
}


