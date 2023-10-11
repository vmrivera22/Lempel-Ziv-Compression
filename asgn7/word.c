#include "word.h"
#include "code.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Word *word_create(uint8_t *syms, uint64_t len) {
  Word *w = (Word *)calloc(1, sizeof(Word));
  w->syms = NULL;
  if(len != 0){
    w->syms = (uint8_t *)calloc(len, sizeof(uint8_t));
    memcpy(w->syms, syms, len); 
  }
  w->len = len;
  return w;
}

Word *word_append_sym(Word *w, uint8_t sym) {
  //if(w == NULL || w->syms == NULL){

  //}
  Word *appended_word = word_create(w->syms, w->len);
  if(appended_word != NULL){
    appended_word->syms = (uint8_t *)realloc(appended_word->syms, sizeof(uint8_t) * (appended_word->len + 1));
    appended_word->len = appended_word->len + 1;
  }
  else{
    appended_word->syms = (uint8_t *)calloc(1, sizeof(uint8_t));
    appended_word->len = 1;
  }
  appended_word->syms[w->len] = sym; // might be -1//////////////////
  return appended_word;
}

void word_delete(Word *w) {
  if(w->syms != NULL){
    free(w->syms);
  }
  free(w);
  return;
}

WordTable *wt_create(void) {
  WordTable *word = (WordTable *)calloc(UINT16_MAX - 1, sizeof(Word));
  word[EMPTY_CODE] = word_create("", 0);
  return word;
}

void wt_reset(WordTable *wt) {
  wt_delete(wt);
  wt = wt_create();
  return;
}

void wt_delete(WordTable *wt) {
  for (int i = 0; i <= UINT16_MAX - 1; i++) {
    if (wt[i] != NULL) {
      word_delete(wt[i]);
    }
  }
  free(wt);
  return;
}
