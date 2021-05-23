#
# Makefile for the UM Test lab
# 
CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -l40locality -lcii40 -lm 

EXECS   = writetests um um_test

all: $(EXECS)

writetests: umlabwrite.o umlab.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um: um.o um_operate.o um_mem.o open_or_die.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um_test: um_test.o um_mem.o um_operate.o open_or_die.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

