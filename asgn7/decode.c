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

// Read and check if the FileHeader is correct.
void decode_header(int infile){
  FileHeader *header = (FileHeader *)calloc(1, sizeof(FileHeader));
  read_header(infile, header);
  if (header->magic != MAGIC) {
    free(header);
    printf("Bad Magic\n");
    exit(1);
  }
  free(header);
  return;
}

// Function decompresses input file.
void decompress_file(WordTable *table, int outfile, int infile){
  uint8_t curr_sym = 0;
  uint16_t curr_code = 0;
  uint16_t next_code = START_CODE;
  // Read pairs from the file until there are no more pairs to read.
  while (read_pair(infile, &curr_code, &curr_sym, bitlen(next_code)) == true) {
    table[next_code] = word_append_sym(table[curr_code], curr_sym); // Append the word/sym to the WordTable.
    buffer_word(outfile, table[next_code]); // Buffer the word - write to outfile.
    next_code += 1;
    if (next_code == MAX_CODE) { // If the max size of code is reached then reset the WordTable. 
      wt_reset(table);
      next_code = START_CODE;
    }
  }
  flush_words(outfile); // Write out any remaining words in the buffer.
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
  bool  display_stats = false;
    int in_file =-2;
  int out_file = -2;

  // Options for program.
  while ((c = getopt(argc, argv, "vi:o:")) != -1) {
    switch (c) {
    case 'v': // Option displays compression status
      display_stats = true;
      break;
    case 'i': // Option allows the user to specify a compressed file.
      input = optarg;
      in_file = open(input, O_RDWR);
      if(in_file < 0){
        fprintf(stderr, "Unable to open input file. Make sure the file exists and try again.\n");
        exit(1);
      }
      break;
    case 'o': // Option allows the user to specify a file to output the decompressed data.
      output = optarg;
      out_file = open(output, O_RDWR | O_TRUNC | O_CREAT, 0666);
      break;
    }
  }

  if(in_file == -2) // Open the input file.
    in_file = copy_stdin();
  if(out_file == -2) // Open the output file.
    out_file = STDOUT_FILENO;

  decode_header(in_file); // read and check FileHeader.
  // Create a word table
  WordTable *table = wt_create();
  decompress_file(table, out_file, in_file); // Decompress the file.
  // Display decompression stats.
  if(display_stats == true){
    extern uint64_t bytes_in;
    extern uint64_t bytes_out;
    printf("Compressed File: %lu B\nDecompressed File: %lu B\nCompression percentage: %f%%\n", bytes_in, bytes_out, (1 - ((float)bytes_out/bytes_in))*100);
    printf("\n");
  }

  // Close files and delete WordTable.
  close(in_file);
  close(out_file);
  wt_delete(table);

  // Remove the temp created file for stdin.
  if(in_file == -2)
    remove("temporary.txt");

  return 0;
}
