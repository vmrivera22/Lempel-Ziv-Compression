#include trie.h

int main(){
    TrieNode *tr = trie_create()
    TrieNode *c1 = trie_node_create(2)
    tr->children['a'] = c1
    TrieNode *rec = trie_step(tr, 'a')
    if(rec == NULL){
        printf("added 'a' (trie_node_create)\n")
    }
    trie_delete(c1)
    TrieNode *rec = trie_step(tr, 'a')
    if(rec == NULL){
        printf("removed 'a' (trie_delete)\n")
    }
    TrieNode *c1 = trie_node_create(2)
    tr->children['a'] = c1
    TrieNode *rec = trie_step(tr, 'a')
    if(rec == NULL){
        printf("added 'a' (trie_node_create)\n")
    }
    trie_reset(tr)
    TrieNode *rec = trie_step(tr, 'a')
    if(rec == NULL){
        printf("reset (trie_reset)\n")
    }
}

