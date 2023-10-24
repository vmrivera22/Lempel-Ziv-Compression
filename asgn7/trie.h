#ifndef __TRIE_H__
#define __TRIE_H__
#include <inttypes.h>

#define ALPHABET 256

typedef struct TrieNode TrieNode;

// Struct definition of a TrieNode
struct TrieNode {
  TrieNode *children[ALPHABET];
  uint16_t code;
};

// Function returns a pointer to a newly created TrieNode that has been allocated memory
TrieNode *trie_node_create(uint16_t code);

// Function deletes a TrieNode
void trie_node_delete(TrieNode *n);

// Function returns a Pointer to a newly initialized Trie: a root TrieNode with the code EMPTY_CODE.
TrieNode *trie_create();

// Dunction resets a Trie to just the root TrieNode.
void trie_reset(TrieNode *root);

// Function deletes a sub-Trie starting from the sub-Trie's root.
void trie_delete(TrieNode *n);

// Function returns a pointer to the TrieNode representing the symbol sym.
TrieNode *trie_step(TrieNode *n, uint8_t sym);

#endif
