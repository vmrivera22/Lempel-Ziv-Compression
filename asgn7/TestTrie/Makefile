FLAGS=-Wall -Wextra -Werror -Wpedantic -g -std=c99
LDFLAGS= -lm
CC=clang $(CFLAGS)

all	:	test test.o trie.o word.o
.PHONY	:	all
test	:	test.o trie.o word.o
		$(CC) $(LDFLAGS) -o test test.o trie.o word.o
test.o	:	test.c
	$(CC) -c test.c
trie.o	:	trie.c
	$(CC) -c trie.c
word.o	:	word.c
	$(CC) -c word.c
clean	:
	rm -rf test test.o trie.o word.o infer-out
infer	:	clean
	infer-capture -- make
	infer-analyze -- make
valgrind	:	test
		valgrind ./test
