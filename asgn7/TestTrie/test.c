#include "trie.h"
#include "word.h"
#include <stdio.h>
#include <stdlib.h>
#include "code.h"
#include <string.h>

int main(){
    WordTable *wt = wt_create();
    Word *w = word_append_sym(wt[EMPTY_CODE], 'a');
    wt[3] = w;
    if(strcmp((const char*) wt[3]->syms, "a") == 0){
        printf("true (word_append_sym, wt_create)\n");
    }
    Word *w2 = word_append_sym(w, 'b');
    wt[4] = w2;
    if(strcmp((const char*) wt[4]->syms, "ab") == 0){
        printf("true true\n");
    }
    wt_reset(wt);
    if(wt[4] == NULL){
       printf("true true (reset)\n");
    }
}

