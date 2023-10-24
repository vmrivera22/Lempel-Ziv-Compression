#include "word.h"
#include "code.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function returns a pointer to a contructed Word that has been allocated memory.
Word *word_create(uint8_t *syms, uint64_t len) {
  Word *w = (Word *)calloc(1, sizeof(Word));
  if(len != 0){ // If the syms is not an empty word, then allocate the apporiate memory (len * uint8_t).
    w->syms = (uint8_t *)calloc(len, sizeof(uint8_t));
    memcpy(w->syms, syms, len);
  }
  else{ // If the sym is an empty word then allocate space for one uint8_t (for the '\0' character)./////////
    w->syms = NULL;//(uint8_t *)calloc(1, sizeof(uint8_t));
  }

  // Copy the sym word and its length to the Word data structure
  w->len = len;
  return w;
}

// Function returns a New Word from the specified Word appended with a symbol.
Word *word_append_sym(Word *w, uint8_t sym) {
  Word *appended_word = word_create(w->syms, w->len); // Create the word that will contain the appended word.
  if(appended_word->syms != NULL){ // If the input word w was not an empty word than reallocate its space to make room for sym.
    appended_word->syms = (uint8_t *)realloc(appended_word->syms, sizeof(uint8_t) * (appended_word->len + 1));
  }
  else{
    appended_word->syms = (uint8_t *)calloc(1, sizeof(uint8_t));
  }
  // Append sym and increase the length of the word.
  appended_word->len = appended_word->len + 1;
  appended_word->syms[w->len] = sym;
  return appended_word;
}

// Function deletes a Word.
void word_delete(Word *w) {
  if(w->syms != NULL){
    free(w->syms);
  }
  free(w);
  return;
}

// Function returns a pointer to a newly created WordTable (array of Words).
WordTable *wt_create(void) {
  WordTable *word = (WordTable *)calloc(UINT16_MAX - 1, sizeof(Word));
  word[EMPTY_CODE] = word_create((uint8_t *) "", 0);
  return word;
}

// Function resets a WordTable to having just the empty Word.
void wt_reset(WordTable *wt) {
  for (int i = EMPTY_CODE + 1; i < UINT16_MAX - 1; i++) { // Delete all words (and set to NULL) in the WordTable but the EMPTY_CODE word.
    if (wt[i] != NULL) {
      word_delete(wt[i]);
      wt[i] = NULL;
    }
  }
  return;
}

// Function deletes and frees memory allocated to a WordTable.
void wt_delete(WordTable *wt) {
  for (int i = 0; i < UINT16_MAX - 1; i++) { // Delete all words in the WordTable.
    if (wt[i] != NULL) {
      word_delete(wt[i]);
      //w[i] = NULL;
    }
  }
  free(wt);
  return;
}

