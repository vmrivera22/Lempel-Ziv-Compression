FLAGS=-Wall -Wextra -Werror -Wpedantic -g -std=c99
LDFLAGS= -lm
CC=clang $(CFLAGS)

all	:	encode decode encode.o decode.o trie.o word.o io.o
.PHONY	:	all
encode	:	encode.o trie.o word.o io.o
		$(CC) $(LDFLAGS) -o encode encode.o trie.o word.o io.o
decode	:	decode.o trie.o word.o io.o
	$(CC) $(LDFLAGS) -o decode decode.o trie.o word.o io.o
encode.o	:	encode.c
	$(CC) -c encode.c
decode.o	:	decode.c
	$(CC) -c decode.c
trie.o	:	trie.c
	$(CC) -c trie.c
word.o	:	word.c
	$(CC) -c word.c
io.o	:	io.c
	$(CC) -c io.c
clean	:
	rm -rf encode encode.o decode decode.o trie.o word.o io.o infer-out
infer	:	clean
	infer-capture -- make
	infer-analyze -- make
valgrind	:	encode
		valgrind ./encode
