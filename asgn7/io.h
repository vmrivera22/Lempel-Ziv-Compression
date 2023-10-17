#ifndef __IO_H__
#define __IO_H__

#include "word.h"
#include <inttypes.h>
#include <stdbool.h>

#define MAGIC 0x8badbeef // Profram's magic number

// Struct definition of a FileHeader
typedef struct FileHeader {
  uint32_t magic;
  uint16_t protection;
} FileHeader;

// Function reads the FileHeader of a infile.
void read_header(int infile, FileHeader *header);

// Function writes the FileHeader to a outfile.
void write_header(int outfile, FileHeader *header);

// Function reads a symbol from the infile and returns true if there are symbols to be read, otherwise returns false.
bool read_sym(int infile, uint8_t *sym);

// Function buffers pairs (code and symbol) and writes them to outfile when the buffer is full. 
void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen);

// Function writes any remaining pairs to the outfile.
void flush_pairs(int outfile);

// Function reads a pair from the infile and returns true if there are any pairs left to read, false otherwise.
bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen);

// Function buffers words and writes them to outfile when the buffer is full.
void buffer_word(int outfile, Word *w);

// Function writes any remaining symbols in the buffer.
void flush_words(int outfile);

int read_buffer(void *buffer, int byte_num, int infile);

int write_buffer(void *buffer, int byte_num, int outfile);

#endif
