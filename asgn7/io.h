#ifndef __IO_H__
#define __IO_H__

#include "word.h"
#include <inttypes.h>
#include <stdbool.h>

#define MAGIC 0x8badbeef

typedef struct FileHeader {
  uint32_t magic;
  uint16_t protection;
} FileHeader;

void read_header(int infile, FileHeader *header);

void write_header(int outfile, FileHeader *header);

bool read_sym(int infile, uint8_t *sym);

void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen);

void flush_pairs(int outfile);

bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen);

void buffer_word(int outfile, Word *w);

void flush_words(int outfile);

#endif
