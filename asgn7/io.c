#include "io.h"
#include "code.h"
#include "endian.h"
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define BUFF_LIMIT 4096
#define CHAR_BITS 8

uint32_t bytes_in = 0;
uint32_t bytes_out = 0;

uint32_t de_bytes_in = 0;
uint32_t de_bytes_out = 0; 

uint8_t out_buff[BUFF_LIMIT+1];
uint32_t out_buff_index = 0;
uint32_t out_buff_ind = 0;

uint8_t word_buff[BUFF_LIMIT+1];
uint32_t word_buff_index = 0;
uint32_t word_buff_ind = 0;

uint32_t read_index = 0;
uint32_t read_ind = 0;

uint8_t sym_buff[BUFF_LIMIT+1];
uint32_t sym_buff_ind = 0;

uint8_t in_buff[BUFF_LIMIT+1];

void read_header(int infile, FileHeader *header) {
  int head_size = sizeof(FileHeader);
  int check = 1;
  int t_bread = 0;

  while((t_bread < head_size) && (check > 0)){
    check = read(infile, header+t_bread, head_size-t_bread);
    t_bread += check;
  }
  if(is_big()){
    uint16_t temp_pro = swap16(header->protection);
    uint32_t temp_mag = swap32(header->magic);
    header->protection = temp_pro;
    header->magic = temp_mag;
  }

  return;
}

void write_header(int outfile, FileHeader *header) {
  if(is_big()){
    uint16_t temp_pro = swap16(header->protection);
    uint32_t temp_mag = swap32(header->magic);
    header->protection = temp_pro;
    header->magic = temp_mag;
  }

  int check = 1;
  int t_bwrite = 0;
  while((t_bwrite < sizeof(FileHeader)) && check != 0){
    check = write(outfile, header+t_bwrite, sizeof(FileHeader)-t_bwrite);
    t_bwrite += check;
  }
   return;
}

bool read_sym(int infile, uint8_t *sym) {
  int temporary;//Temporary to hold the value of syms bits.
  if (sym_buff_ind == 0 || sym_buff_ind == BUFF_LIMIT) {//read a block of code into buffer.
    memset(sym_buff, '\0', BUFF_LIMIT);
    int check = 1;
    int t_bread = 0;
    while((t_bread < BUFF_LIMIT) && (check != 0)){
      check = read(infile, sym_buff+t_bread, BUFF_LIMIT-t_bread);
      t_bread += check;
    }
    bytes_in += t_bread;//to_read;
    sym_buff_ind = 0;
  }
  for (int j = 0; j < CHAR_BITS; j++) {
    temporary = ((sym_buff[sym_buff_ind] & (0x1 << (j))) >> (j));
    if (temporary == 1) {//if the bit is set, set it in the buffer.
      sym_buff[sym_buff_ind] = sym_buff[sym_buff_ind] | (0x1 << (j));
    }
  }
  *sym = sym_buff[sym_buff_ind];
  if (*sym == 0) {
    return false;//If there is nothing left to read return false.
  }
  sym_buff_ind++;
  return true;
}


void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen) {
  uint16_t temp;
  for (int i = 0; i < bitlen; i++) {
    if((out_buff_index) == (BUFF_LIMIT*8)){
      bytes_out += (out_buff_index/8);
      write(outfile, out_buff, BUFF_LIMIT);
      memset(out_buff, '\0', BUFF_LIMIT);
      out_buff_index = 0;
    }
    temp = (((code) & (0x1 << (i))) >> (i));
    if(temp == 1){
       out_buff[out_buff_index / 8] = out_buff[out_buff_index / 8] | (0x1 << (out_buff_index % 8));
    }
    out_buff_index++;
  }

  for (int j = 0; j < CHAR_BITS; j++) {
    if((out_buff_index) == (BUFF_LIMIT*8)){
      bytes_out += (out_buff_index/8);
      write(outfile, out_buff, BUFF_LIMIT);
      memset(out_buff, '\0', BUFF_LIMIT);
      out_buff_index = 0;
    }
    temp = (((sym) & (0x1 << (j))) >> (j));//The same as for code but for the sym.
    if(temp == 1){
      out_buff[out_buff_index / 8] = out_buff[out_buff_index / 8] | (0x1 << (out_buff_index % 8));     
    }
    out_buff_index++;
  }
  return;
}


void flush_pairs(int outfile) {
  write(outfile, out_buff, (out_buff_index / 8));
  memset(out_buff, '\0', BUFF_LIMIT);
  out_buff_index = 0;
  out_buff_ind = 0;
  bytes_out = bytes_out + ((out_buff_index / 8));
  return;
}


bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen) {
  if ((read_index == 0) || (read_index >= (BUFF_LIMIT*8))) {
    memset(in_buff, '\0', BUFF_LIMIT);
    int check = 1;
    int t_bread = 0;
    while((t_bread < BUFF_LIMIT) && (check > 0)){
      check = read(infile, in_buff+t_bread, BUFF_LIMIT-t_bread);
      t_bread += check;
    }
    de_bytes_in += t_bread;
    read_index = 0;
    read_ind = 0;
  }

  uint16_t temp_code = 0;
  for (int i = 0; i < bitlen; i++) {
    if(read_index >=  (BUFF_LIMIT*8)){
      memset(in_buff, '\0', BUFF_LIMIT);
      int check = 1;
      int t_bread = 0;
      while((t_bread < BUFF_LIMIT) && (check > 0)){
        check = read(infile, in_buff+t_bread, BUFF_LIMIT-t_bread);
        t_bread += check;
      }
      read_index = 0;
      read_ind = 0;
    }
    int binary = ((in_buff[read_index / 8] & (0x1 << (read_index % 8))) >> (read_index % 8));
    if (binary == 1) {
      temp_code = temp_code | (0x1 << i);
    }
    read_index++;
  }
  read_ind += bitlen / 8;
  *code = temp_code;
  uint8_t temp_sym = 0;
  for (int j = 0; j < CHAR_BITS; j++) {
    if(read_index >=  (BUFF_LIMIT*8)){
      memset(in_buff, '\0', BUFF_LIMIT);
      int check = 1;
      int t_bread = 0;
      while((t_bread < BUFF_LIMIT) && (check > 0)){
        check = read(infile, in_buff+t_bread, BUFF_LIMIT-t_bread);
        t_bread += check;
      }
      read_index = 0;
      read_ind = 0;
    }
    int binary = ((in_buff[read_index / 8] & (0x1 << (read_index % 8))) >> (read_index % 8));
    if (binary == 1) {
     temp_sym = temp_sym | (0x1 << j);
    }
    read_index++;
  }
  read_ind += CHAR_BITS / 8;
  *sym = temp_sym;

  if (*code == STOP_CODE) {
    return false;//If there is no more to read then return false.
  }
  return true;
}



void buffer_word(int outfile, Word *w) {
  int byte_num = w->len;
  for (int i = 0; i < byte_num; i++) {
    for (int j = 0; j < CHAR_BITS; j++) {
      int temp = ((w->syms[i] & (0x1 << (j))) >> (j));//Keeps track if the bit is set.
      if (temp == 1) {
        word_buff[word_buff_ind] = word_buff[word_buff_ind] | (0x1 << (j));//Put it in the buffer.
      }
    }
    word_buff_ind++;
    if (word_buff_ind == BUFF_LIMIT) {//If the buffer is full write out the buffer.
      write(outfile, word_buff, BUFF_LIMIT);
      memset(word_buff, '\0', BUFF_LIMIT);
      de_bytes_out = de_bytes_out + BUFF_LIMIT;
      word_buff_ind = 0;
    }
  }
  return;
}
void flush_words(int outfile) {
  de_bytes_out = de_bytes_out + word_buff_ind;
  write(outfile, word_buff, word_buff_ind);
  memset(word_buff, '\0', BUFF_LIMIT);
  word_buff_ind = 0;
  return;
}
