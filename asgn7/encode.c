#include "code.h"
#include "io.h"
#include "trie.h"
#include "word.h"
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define buff_size 4096


uint8_t bitlen(uint16_t x) {
  if (x != 0) {
    uint32_t count = 0;
    while(x){
      count++;
      x >>= 1;
    }
    return count;
  } else {
    return 1;
  }
}

int main(int argc, char **argv) {
  char *input;
  char *output;
  bool in = false;
  bool out = false;
  int c;
  bool display_stats = false;
   display_stats = false;
  while ((c = getopt(argc, argv, "vi:o:")) != -1) {
    switch (c) {
    case 'v':
           display_stats = true;
      break;
    case 'i':
      in = true;
      input = optarg;
      break;
    case 'o':
      out = true;
      output = optarg;
      break;
    }
  }
  if(in == false){
  int temp_file = open("temporary.txt",  O_RDWR | O_CREAT);
  uint8_t *file_out = (uint8_t *)calloc(4096, sizeof(uint8_t));
  int buffer_size = buff_size;
  int count = 0;
  int j;
  while((j = read(STDIN_FILENO, file_out, 1)) > 0){
    count++;
    write(temp_file, file_out, 1);
    if(count == buffer_size){
      buffer_size = buffer_size * 2;
      file_out = (uint8_t *)realloc(file_out, buffer_size);
    }
  }
   close(temp_file);
  }
  int in_file;
  int out_file;

  if(in == true){
    in_file = open(input, O_RDWR);
  }
  else{
    in_file = open("temporary.txt", O_RDWR);
  }
  if(out == true){
    out_file = open(output, O_RDWR);
  }
  else{
    out_file = STDOUT_FILENO;
  }
  struct stat info;
  fstat(in_file, &info);
 
  // Write and Free header
  FileHeader *header = (FileHeader*)calloc(1, sizeof(FileHeader));
  header->magic = MAGIC;
  header->protection = info.st_mode;
  write_header(out_file, header);
  free(header);

  bool mult = false;
  TrieNode *root = trie_create();
 
  TrieNode *curr_node = root;
  TrieNode *prev_node = NULL;
  TrieNode *next_node = NULL;
  uint8_t curr_sym = 0;
  uint8_t prev_sym = 0;
  uint16_t next_code = START_CODE;

  while (read_sym(in_file, &curr_sym) == true) {  
    next_node = trie_step(curr_node, curr_sym);
    if (next_node != NULL) {
      prev_node = curr_node;
      curr_node = next_node;
    } else {
      buffer_pair(out_file, curr_node->code, curr_sym, bitlen(next_code)); ///
      curr_node->children[curr_sym] = trie_node_create(next_code); ///
      curr_node = root;
      next_code = next_code + 1;
    } 
    if (next_code == MAX_CODE) {
      trie_reset(root);
      curr_node = root;
      next_code = START_CODE;
    }
 
    prev_sym = curr_sym;
  }
  if (curr_node != root) {
    buffer_pair(out_file, prev_node->code, prev_sym, bitlen(next_code));
    next_code = (next_code + 1) % MAX_CODE;
  }
  buffer_pair(out_file, STOP_CODE, 0, bitlen(next_code));
  flush_pairs(out_file);
  if(in == false){
    remove("temporary.txt");
  }
 
  if(display_stats == true){
  extern double bytes_in;
  extern double bytes_out;
  printf("Bytes in: %f\nBytes out: %f\nPercent: %f\n", bytes_in, bytes_out, bytes_in/bytes_out);
  printf("\n");
  }
 
  trie_delete(root);
  close(in_file);
  close(out_file);
}
