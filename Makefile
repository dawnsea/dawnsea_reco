#gcc -o test pre_calc.c simi_euc.c rbtree.c rbtree_util.c main.c test_all.c input_file.c debug.c store_data.c store_item.c multi_loader.c -lm -lpthread -DDEBUG -DDEBUG_INFO -DDEBUG_DEBUG -Wall
CC = gcc -O3 -Wall -g

RECO_SRC = $(wildcard *.c)
DEBUG = -DDEBUG -DDEBUG_INFO -DDEBUG_DEBUG -Wall
LIBS = -lm -lpthread

all: $(RECO_SRC)
	$(CC) -o reco_test $(RECO_SRC) $(DEBUG) $(LIBS)
	
	
.SUFFIXES: .c.o
	
.c.o:
	$(CC) -c -o $@ $< $(DEBUG) $(LIBS)
	


clean :
	find ./ -name "*.*o" -exec rm -f {} +
	rm -f reco_test
