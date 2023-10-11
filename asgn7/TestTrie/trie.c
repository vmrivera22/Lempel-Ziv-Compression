#include "trie.h"
#include "code.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

TrieNode *trie_node_create(uint16_t code) {
  TrieNode *trie = (TrieNode *)calloc(1, sizeof(TrieNode));
  for (int i = 0; i < ALPHABET; i++) { // Make all node children NULL
    trie->children[i] = NULL;
  }
  trie->code = code; // Code of the Node
  return trie;
}

void trie_node_delete(TrieNode *n) {
  free(n);
  return;
}

TrieNode *trie_create(void) {
  TrieNode *trie = trie_node_create(EMPTY_CODE);
  return trie;
}

void trie_reset(TrieNode *root) {
  int i;
  for (i = 0; i < ALPHABET; i++) {
    if (root->children[i] != NULL) {
      trie_reset(root->children[i]);
      trie_node_delete(root->children[i]);
      root->children[i] = NULL;
    }
  }
  return;
}

void trie_delete(TrieNode *n) {
  int i;
  for (i = 0; i < ALPHABET; i++) {
    if (n->children[i] != NULL) {
      trie_delete(n->children[i]);
      trie_node_delete(n->children[i]);
    }
  }
  trie_node_delete(n);
  return;
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
  if(n != NULL){
      return n->children[sym];
  }
  return NULL;
    
}
