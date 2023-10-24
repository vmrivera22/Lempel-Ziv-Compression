#ifndef __WORD_H__
#define __WORD_H__

#include <inttypes.h>

// Struct definition of a Word
typedef struct Word {
  uint8_t *syms;
  uint64_t len;
} Word;

typedef Word *WordTable;

// Function returns a pointer to a contructed Word that has been allocated memory.
Word *word_create(uint8_t *syms, uint64_t len);

// Function returns a New Word from the specified Word appended with a symbol.
Word *word_append_sym(Word *w, uint8_t sym);

// Function deletes a Word.
void word_delete(Word *w);

// Function returns a pointer to a newly created WordTable (array of Words).
WordTable *wt_create();

// Function resets a WordTable to having just the empty Word.
void wt_reset(WordTable *wt);

// Function deletes and frees memory allocated to a WordTable.
void wt_delete(WordTable *wt);

#endif
