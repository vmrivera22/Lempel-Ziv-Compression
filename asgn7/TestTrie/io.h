//Code by Darrell Long from Assignment 7 PDF

#ifndef __IO_H__
#define __IO_H__

#include "word.h"
#include <inttypes.h>
#include <stdbool.h>

#define MAGIC 0x8badbeef

// Struct definition of a FileHeader.
typedef struct FileHeader {
  uint32_t magic;
  uint16_t protection;
} FileHeader;

// Reads in sizeof (FileHeader) bytes from the input file.
// These bytes are read into the supplied FileHeader, header.
// Endianness is swapped if byte order isn't little endian
void read_header(int infile, FileHeader *header);

// Writes sizeof(FileHeader) bytes to the output file.
// These bytes are from the supplied FileHeader, header.
// Endianness is swapped if byte order isn't little endian.
void write_header(int outfile, FileHeader *header);

// " Reads " a symbol from the input file .
// The " read " symbol is placed into the pointer to sym . (e.g. * sym = val )
// In reality , a block of symbols is read into a buffer .
// An index keeps track of the currently read symbol in the buffer .
// Once all symbols are processed , another block is read .
// If less than a block is read , the end of the buffer is updated .
// Returns true if there are symbols to be read , false otherwise .
bool read_sym(int infile, uint8_t *sym);


// Buffers a pair . A pair is comprised of a code and a symbol .
// The bits of the code are buffered first , starting from the LSB .
// The bits of the symbol are buffered next , also starting from the LSB .
// The code buffered has a bit - length of bitlen .
// The buffer is written out whenever it is filled .
void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen);

// Writes out any remaining pairs of symbols and codes to the output file .
void flush_pairs(int outfile);

// " Reads " a pair ( code and symbol ) from the input file .
// The " read " code is placed in the pointer to code (e.g. * code = val )
// The " read " symbol is placed in the pointer to sym (e.g. * sym = val ).
// In reality , a block of pairs is read into a buffer .
// An index keeps track of the current bit in the buffer .
// Once all bits have been processed , another block is read .
// The first bitlen bits are the code , starting from the LSB .
// The last 8 bits of the pair are the symbol , starting from the LSB .
// Returns true if there are pairs left to read in the buffer , else false .
// There are pairs left to read if the read code is not STOP_CODE .
bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen);

// Buffers a Word , or more specifically , the symbols of a Word .
// Each symbol of the Word is placed into a buffer .
// The buffer is written out when it is filled .
void buffer_word(int outfile, Word *w);

// Writes out any remaining symbols in the buffer.
void flush_words(int outfile);

#endif
