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

#define BUFF_SIZE 4096

// Function returns the bit length of a uint16_t value.
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

// Write and Free header
void encode_header(int outfile, int infile){
  struct stat info;
  fstat(infile, &info);
  FileHeader *header = (FileHeader*)calloc(1, sizeof(FileHeader));
  header->magic = MAGIC;
  header->protection = info.st_mode;
  write_header(outfile, header);
  free(header);
}

// Function compresses input file.
void compress_file(TrieNode *root, int outfile, int infile){
  TrieNode *curr_node = root;
  TrieNode *prev_node = NULL;
  TrieNode *next_node = NULL;
  uint8_t curr_sym = 0;
  uint8_t prev_sym = 0;
  uint16_t next_code = START_CODE;

  // Read symbols/words from the infile until there is no more to read.
  while (read_sym(infile, &curr_sym) == true) {  
    next_node = trie_step(curr_node, curr_sym);
    if (next_node != NULL) { // If the symbol curr_sym exists as a child of curr_node then step down the trie.
      prev_node = curr_node;
      curr_node = next_node;
    } else { // If not then buffer the current sym and add it as a child of curr_node
      buffer_pair(outfile, curr_node->code, curr_sym, bitlen(next_code));
      curr_node->children[curr_sym] = trie_node_create(next_code);
      curr_node = root;
      next_code = next_code + 1;
    } 
    if (next_code == MAX_CODE) { // If the max size of code is reached then reset the trie
      trie_reset(root);
      curr_node = root;
      next_code = START_CODE;
    }
    prev_sym = curr_sym;
  }

  // Buffer the last pair in the file (if it is not the root).
  if (curr_node != root) {
    buffer_pair(outfile, prev_node->code, prev_sym, bitlen(next_code));
    next_code = (next_code + 1) % MAX_CODE;
  }

  // Add the STOP_CODE to the end of the compressed file.
  buffer_pair(outfile, STOP_CODE, 0, bitlen(next_code));
  flush_pairs(outfile);
  return;
}

// Function returns the file descriptor of the newly created temp file containing STDIN data.
int copy_stdin(){
  int temp_file = open("temporary.txt",  O_RDWR | O_CREAT); // O_TMPFILE
  uint8_t *file_out = (uint8_t *)calloc(4096, sizeof(uint8_t)); // Buffer to hold stdin input before writing to file.
  int buffer_size = BUFF_SIZE;
  int count = 0;
  while((read(STDIN_FILENO, file_out, 1)) > 0){ // Read from stdin
    count++;
    write(temp_file, file_out, 1);
    if(count == buffer_size){ // If the buffer is full then double its size.
      buffer_size = buffer_size * 2;
      file_out = (uint8_t *)realloc(file_out, buffer_size);
    }
  }
  return temp_file;
}

int main(int argc, char **argv) {
  char *input;
  char *output;
  int c;
  bool display_stats = false;
  int in_file = -2;
  int out_file = -2;

  // Options for program.
  while ((c = getopt(argc, argv, "vi:o:")) != -1) {
    switch (c) {
    case 'v': // Option displays compression stats.
      display_stats = true;
      break;
    case 'i': // Option allows the user to specify a file to compress.
      input = optarg;
      in_file = open(input, O_RDWR); // Open input file
      break;
    case 'o': // Option allows the user to specify a file to output compressed data.
      output = optarg;
      out_file = open(output, O_RDWR | O_TRUNC | O_CREAT, 0666); // Open output file
      break;
    }
  }

  if(in_file == -2)
    in_file = copy_stdin();
  if(out_file == -2)
    out_file = STDOUT_FILENO; // Set the program to output to stdout.

  encode_header(out_file, in_file); // Write header to out_file.

  TrieNode *root = trie_create();
  compress_file(root, out_file, in_file); // Compress the file
 
  // Display compression stats.
  if(display_stats == true){
    //extern double bytes_in;
    //extern double bytes_out;
    extern uint64_t bytes_in;
    extern uint64_t bytes_out;
    printf("Original File: %lu B\nCompressed File: %lu B\nCompression percentage: %f%%\n", bytes_in, bytes_out, (1 - ((float)bytes_out/bytes_in))*100);
    printf("\n");
  }
 
  // Close files and delete trie.
  trie_delete(root);
  close(in_file);
  close(out_file);

  // Remove the temp created file for stdin.
  if(in_file == -2)
    remove("temporary.txt");
  
  return 0;
}
