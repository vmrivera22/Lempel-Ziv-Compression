//Code provided by Darrell Long Assignment 7 PDF

#ifndef __TRIE_H__
#define __TRIE_H__
//#include "util.h"
#include <inttypes.h>

#define ALPHABET 256

typedef struct TrieNode TrieNode;

struct TrieNode {
  TrieNode *children[ALPHABET];
  uint16_t code;
};

// Constructor for a TrieNode
TrieNode *trie_node_create(uint16_t code);

// Destructor for a TrieNode
void trie_node_delete(TrieNode *n);

// Initializes a Trie: a root TrieNode with the code EMPTY_CODE.
TrieNode *trie_create(void);

// Resets a Trie to just the root TrieNode
void trie_reset(TrieNode *root);

// Deletes a sub-Trie starting from the sub-Trie's root.
void trie_delete(TrieNode *n);

// Returns a pointer to the child TrieNode representing the symbol sym
// If the symbol doesn't exist, NULL is returned.
TrieNode *trie_step(TrieNode *n, uint8_t sym);

#endif
