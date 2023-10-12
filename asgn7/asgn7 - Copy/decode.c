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
  bool  display_stats = false;
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

   int in_file;
   int out_file;
   if(in == true){
  in_file = open(input, O_RDWR);
   }
   else{
     in_file = STDIN_FILENO;
   }
   if(out == true){
  out_file = open(output, O_RDWR);
   }
   else{
     out_file = STDOUT_FILENO;
   }

  FileHeader *header = (FileHeader *)calloc(1, sizeof(FileHeader));
  read_header(in_file, header);
  if (header->magic != MAGIC) {
    printf("Bad Magic\n");
    return 1;
  }
  WordTable *table = wt_create();
  uint8_t curr_sym = 0;
  uint16_t curr_code = 0;
  uint16_t next_code = START_CODE;
  
  while (read_pair(in_file, &curr_code, &curr_sym, bitlen(next_code)) == true) {
      table[next_code] = word_append_sym(table[curr_code], curr_sym);
      buffer_word(out_file, table[next_code]);
      next_code = next_code + 1;
    if (next_code == MAX_CODE) {
      wt_reset(table);
      next_code = START_CODE;
    }
  }
   if(display_stats == true){
  extern double de_bytes_in;
  extern double de_bytes_out;
  printf("Bytes in: %f\nBytes out: %f\nPercent: %f\n", de_bytes_in, de_bytes_out, de_bytes_in/de_bytes_out);
  printf("\n");
  }
  flush_words(out_file);
  close(in_file);
  close(out_file);
  wt_delete(table);
}
