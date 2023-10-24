#include "trie.h"
#include "code.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Function returns a pointer to a newly created TrieNode that has been allocated memory
TrieNode *trie_node_create(uint16_t code) {
  TrieNode *trie = (TrieNode *)calloc(1, sizeof(TrieNode));
  for (int i = 0; i < ALPHABET; i++) { // Create a TrieNode and set its children to NULL.
    trie->children[i] = NULL;
  }
  trie->code = code;
  return trie;
}

// Function deletes a TrieNode
void trie_node_delete(TrieNode *n) {
  free(n);
  return;
}

// Function returns a Pointer to a newly initialized Trie: a root TrieNode with the code EMPTY_CODE.
TrieNode *trie_create() {
  TrieNode *trie = trie_node_create(EMPTY_CODE);
  return trie;
}

// Dunction resets a Trie to just the root TrieNode.
void trie_reset(TrieNode *root) {
  for (int i = 0; i < ALPHABET; i++) {
    if (root->children[i] != NULL) {
      trie_reset(root->children[i]); // Recursively delete the trie's children
      trie_node_delete(root->children[i]); // Leaves the root Node intact by passing in root->children[i].
      root->children[i] = NULL;
    }
  }
  return;
}

// Function deletes a sub-Trie starting from the sub-Trie's root.
void trie_delete(TrieNode *n) {
  for (int i = 0; i < ALPHABET; i++) {
    if (n->children[i] != NULL) {
      trie_delete(n->children[i]); // Recursively delete nodes.
    }
  }
  if(n != NULL){
    trie_node_delete(n); // Delete all nodes that have been visited by the recursion including the input node.
  }
  return;
}

// Function returns a pointer to the TrieNode representing the symbol sym.
TrieNode *trie_step(TrieNode *n, uint8_t sym) {
  if(n != NULL){
      return n->children[sym];
  }
  return NULL; 
}
