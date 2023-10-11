FLAGS=-Wall -Wextra -Werror -Wpedantic -g -std=c99
LDFLAGS= -lm
CC=clang $(CFLAGS)

all	:	test test.o trie.o
.PHONY	:	all
test	:	test.o trie.o
		$(CC) $(LDFLAGS) -o test test.o trie.o
test.o	:	test.c
	$(CC) -c test.c
trie.o	:	trie.c
	$(CC) -c trie.c
clean	:
	rm -rf test test.o trie.o infer-out
infer	:	clean
	infer-capture -- make
	infer-analyze -- make
valgrind	:	test
		valgrind ./test
