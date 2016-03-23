CC = g++
CPPFLAGS = -c -Wall -pedantic -m64 -g -std=c++11
LDFLAGS = -m64

a.out:					main.o game.o deck.o card_node.o hand_play_hum_comp.o
						$(CC) *.o -o a.out

main.o:					main.cpp cs202homework1.h
						$(CC) $(CPPFLAGS) main.cpp

game.o:					game.cpp cs202homework1.h
						$(CC) $(CPPFLAGS) game.cpp

deck.o:					deck.cpp cs202homework1.h
						$(CC) $(CPPFLAGS) deck.cpp

card_node.o:			card_node.cpp cs202homework1.h
						$(CC) $(CPPFLAGS) card_node.cpp

hand_play_hum_comp.o:	hand_play_hum_comp.cpp cs202homework1.h
						$(CC) $(CPPFLAGS) hand_play_hum_comp.cpp

.PHONY: check
check:					# check for memory leak
						$(info -- Checking For Memory Leaks --)
						valgrind --leak-check=full ./a.out

.PHONY: debug
debug:					# check for memory leak
						$(info -- Debugging --)
						gdb ./a.out

.PHONY: clean
clean:					# clean the directory
						$(info -- Cleaning The Directory --)
						rm -rf *.o a.out

.PHONY: run
run:					# clean the directory
						$(info -- Running Program --)
						./a.out
