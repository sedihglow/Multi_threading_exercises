CC = gcc
CPPFLAGS = -c -Wall -Wextra -pedantic -O0 -g 
LDFLAGS = -pthread -m64

a.out:					threadedInc.o err_handle.o get_num.o
						$(CC) $(LDFLAGS) *.o -o a.out

threadedInc.o:			threadedInc.c ../../../sedhead.h
						$(CC) $(CPPFLAGS) threadedInc.c

err_handle.o:			../../../err_handle/err_handle.c ../../../sedhead.h
						$(CC) $(CPPFLAGS) ../../../err_handle/err_handle.c

get_num.o:				../../../get_num/get_num.c ../../../sedhead.h
						$(CC) $(CPPFLAGS) ../../../get_num/get_num.c

.PHONY: check
check:					# check for memory leak
						$(info -- Checking For Memory Leaks --)
						valgrind --leak-check=full ./a.out

.PHONY: debug
debug:					# GNU debugger
						$(info -- Debugging --)
						gdb ./a.out

.PHONY: clean
clean:					# clean the directory
						$(info -- Cleaning The Directory --)
						rm -rf *.o a.out

.PHONY: run
run:					# run the program as follows
						$(info -- Running Program --)
						./a.out
