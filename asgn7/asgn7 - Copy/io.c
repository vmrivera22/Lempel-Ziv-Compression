#include "io.h"
#include "code.h"
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFF_LIMIT 4096
#define CHAR_BITS 8

uint32_t bytes_in = 0;
uint32_t bytes_out = 0;
uint32_t buff_index = 0;//index and their buffers.
uint32_t buff_2_index = 0;
uint32_t read_index = 0;
uint32_t buff_ind = 0;
uint32_t de_bytes_in = 0;
uint32_t de_bytes_out = 0; 
uint32_t buff_2_ind = 0;
uint32_t read_ind = 0;
uint32_t read_index_2 = 0;
uint32_t read_ind_2 = 0;
uint8_t read_buffer[BUFF_LIMIT];
uint8_t buffer[BUFF_LIMIT];
uint8_t buff_2[BUFF_LIMIT];

void read_header(int infile, FileHeader *header) {
  int head_size = sizeof(FileHeader);
  int check = 1;
  while((head_size > 0) && (check != 0)){
    check = read(infile, header, head_size);
    head_size -= check;
  }
  return;
}

void write_header(int outfile, FileHeader *header) {
  int head_size = sizeof(FileHeader);
  while(head_size > 0){
    int check = write(outfile, header, head_size);
    head_size -= check;
  }
   return;
}

bool read_sym(int infile, uint8_t *sym) {
  int temporary;//Temporary to hold the value of syms bits.
  if (read_index_2 == 0 || read_ind_2 == BUFF_LIMIT) {//read a block of code into buffer.
    //int to_read = BUFF_LIMIT;
    //int check = 1;
    memset(read_buffer, '\0', BUFF_LIMIT);
    //while((to_read > 0) && (check != 0)){
    int check = read(infile, read_buffer, BUFF_LIMIT);
    //  to_read -= check;
    //}
    bytes_in = bytes_in + to_read;
    read_index_2 = 0;
    read_ind_2 = 0;
  }
  for (int j = 0; j < CHAR_BITS; j++) {
    temporary = ((read_buffer[read_ind_2] & (0x1 << (j))) >> (j));
    if (temporary == 1) {//if the bit is set, set it in the buffer.
      read_buffer[read_ind_2] = read_buffer[read_ind_2] | (0x1 << (j));
    }
    read_index_2++;
  }
  *sym = read_buffer[read_ind_2];
  if (*sym == 0) {
    return false;//If there is nothing left to read return false.
  }
  read_ind_2++;
  return true;
}

void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen) {
  int i;
  int bytess = (bitlen / 8) + 1;  
  uint16_t temporary;
  if (buff_ind == BUFF_LIMIT) {
     //printf("here\n");
    bytes_out = bytes_out + BUFF_LIMIT;
    int y = write(outfile, buffer, BUFF_LIMIT);//If the buffer is full write it out.
    buff_index = 0;
    buff_ind = 0;
  }
  int e = bitlen - 1;
  for (i = 0; i < bitlen; i++) {
    temporary = (((code) & (0x1 << (i))) >> (i));
    if(temporary == 1){
       buffer[buff_index / 8] = buffer[buff_index / 8] | (0x1 << (buff_index % 8));

    }
    buff_index++;
    e--;
  }
  buff_ind++;

  int bits = CHAR_BITS;
  e = bits - 1;

  for (int j = 0; j < bits; j++) {
    temporary = (((sym) & (0x1 << (j))) >> (j));//The same as for code but for the sym.
    if(temporary == 1){
      buffer[buff_index / 8] = buffer[buff_index / 8] | (0x1 << (buff_index % 8));     
    }
    e--;
    buff_index++;
  }
  buff_ind++;
  return;
}

void flush_pairs(int outfile) {
  int t = write(outfile, buffer, (buff_index / 8));
  bytes_out = bytes_out + ((buff_index / 8));
  return;
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen) {
  uint8_t it[BUFF_LIMIT];
  uint8_t sym_temp[BUFF_LIMIT];
  uint8_t temp[BUFF_LIMIT];
  uint32_t shift_by = bitlen - 1;
  int bytess = (bitlen / 8) + 1;
  if (read_ind == 0 || read_ind == BUFF_LIMIT) {
    int d = read(infile, it, BUFF_LIMIT);
    de_bytes_in = de_bytes_in + d;
    read_index = 0;
    read_ind = 0;
  }

  uint16_t bin_temp = 0;
  for (int i = 0; i < bitlen; i++) {
    int binary = ((it[read_index / 8] & (0x1 << (read_index % 8))) >> (read_index % 8));
    if (binary == 1) {
      bin_temp = bin_temp | (0x1 << i);
    }
    shift_by--;
    read_index++;
  }
  *code = bin_temp;
  read_ind++;
  shift_by = CHAR_BITS - 1;//a symbol has only 8 bits so you shift from 0 - 7.
  uint8_t bi_temp = 0;
  for (int i = 0; i < CHAR_BITS; i++) {
    int binary = ((it[read_index / 8] & (0x1 << (read_index % 8))) >> (read_index % 8));
    if (binary == 1) {
     bi_temp = bi_temp | (0x1 << i);
    }
    read_index++;
    shift_by--;
  }
  *sym = bi_temp;
  read_ind++;

  if (*code == STOP_CODE) {
    return false;//If there is no more to read then return false.
  }
  return true;
}

void buffer_word(int outfile, Word *w) {
  int temporary;
  int byte_num = w->len;
  for (int i = 0; i < byte_num; i++) {
    for (int j = 0; j < CHAR_BITS; j++) {
      temporary = ((w->syms[i] & (0x1 << (j))) >> (j));//Keeps track if the bit is set.
      if (temporary == 1) {
        buff_2[buff_2_ind] = buff_2[buff_2_ind] | (0x1 << (j));//Put it in the buffer.
      }
    buff_2_index++;
    }
    buff_2_ind++;
    if (buff_2_ind == BUFF_LIMIT) {//If the buffer is full write out the buffer.
      write(outfile, buff_2, BUFF_LIMIT);
      buff_2_index = 0;
      de_bytes_out = de_bytes_out + BUFF_LIMIT;
      buff_2_ind = 0;
    }
  }
  return;
}
void flush_words(int outfile) {
  de_bytes_out = de_bytes_out + buff_2_ind;
  write(outfile, buff_2, buff_2_ind);//If the buffer is not full but the end of the file has been reached, write out the amount of bits that the buffer has.
  return;
}
