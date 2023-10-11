//Code provided by Darrell Long Assignmrnt 7 PDF

#ifndef __WORD_H__
#define __WORD_H__

#include <inttypes.h>

// Struct definition of a Word
typedef struct Word {
  uint8_t *syms;
  uint64_t len;
} Word;

typedef Word *WordTable;

// Constructor for a word.
Word *word_create(uint8_t *syms, uint64_t len);

// Constructs a new Word from the specified Word appended with a symbol.
// The Word specified to append ot may be empy.
// If the above is the case, the new Word should contain only the symbol.
Word *word_append_sym(Word *w, uint8_t sym);

// Destructor for a Word.
void word_delete(Word *w);

// Creates a new WordTable, which is an array of Words.
// A WordTable has a pre-defined size of MAX_CODE (UINT16_MAX-1).
// This is because codes are 16-bit integers.
// This Word represents the empty word, a string of length of zero.
WordTable *wt_create(void);

// Resets a WordTable to having just the empty Word.
void wt_reset(WordTable *wt);

// Deletes an entire WordTable.
// All Words in the WordTable must be deleted as well.
void wt_delete(WordTable *wt);

#endif
