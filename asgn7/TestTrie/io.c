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
#define BUFF_LIMIT 4096
#define CHAR_BITS 8

//uint32_t bytes_out = 0;
uint32_t word_buff_index = 0;

//uint32_t de_bytes_in = 0;
//uint32_t de_bytes_out = 0; 

uint32_t word_buff_ind = 0;

uint32_t buff_ind = 0;
uint32_t buff_index = 0;
uint32_t read_ind = 0;
uint32_t read_index = 0;

uint32_t read_sym_ind = 0;

uint8_t read_buffer[BUFF_LIMIT+1];
uint8_t buffer[BUFF_LIMIT+1];
uint8_t in_buffer[BUFF_LIMIT+1];

uint8_t word_buff[BUFF_LIMIT+1];

void read_header(int infile, FileHeader *header) {
  int head_size = sizeof(FileHeader);
  int check = 1;
  int t_bread = 0;
  while((t_bread < head_size) && (check != 0)){
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
  int head_size = sizeof(FileHeader);
  while(head_size > 0){
    int check = write(outfile, header, head_size);
    head_size -= check;
  }
   return;
}

// read_buffer
bool read_sym(int infile, uint8_t *sym){
  int temporary;//Temporary to hold the value of syms bits.
  if(read_sym_ind == BUFF_LIMIT){
    memset(read_buffer, '\0', BUFF_LIMIT);
    int check = 1;
    int t_read = 0;
    while((t_read < BUFF_LIMIT) && (check != 0)){
        check = read(infile, read_buffer+t_read, BUFF_LIMIT-t_read);
        t_read += check;
    }
    read_sym_ind = 0;  
  }
    
  for (int j = 0; j < CHAR_BITS; j++) {
      temporary = ((read_buffer[read_sym_ind] & (0x1 << (j))) >> (j));
      if (temporary == 1) {//if the bit is set, set it in the buffer.
        read_buffer[read_sym_ind] = read_buffer[read_sym_ind] | (0x1 << (j));
      }
  }
  *sym = read_buffer[read_sym_ind];
  if (*sym == 0) {
    return false;//If there is nothing left to read return false.
  }
  read_sym_ind++;
  return true;
}
/*
void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen) {
  int i;
  //int by = (bitlen / 8) + 1;  
  uint16_t temporary;
  if (buff_ind == BUFF_LIMIT) {
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
*/
void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen) {
  uint32_t to_add = (bitlen / 8) + (CHAR_BITS / 8);
  if((buff_ind + to_add) >= BUFF_LIMIT){
    write(outfile, buffer, BUFF_LIMIT);
    memset(buffer, '\0', BUFF_LIMIT);
    buff_ind = 0;
    buff_index = 0;
  }

  // Buffer bits of the code starting form the LSB
  uint16_t temp;
  for(int i = bitlen-1; i >= 0; i--){
    temp = (((code) & (0x1 << (i))) >> (i));
    if(temp == 1){
      buffer[buff_index / 8] = buffer[buff_index / 8] | (0x1 << (buff_index % 8));
    }
    buff_index++;
  }

  buff_ind += bitlen / 8;
  
  // Buffer bits of the symbol strating from the LSB
  for(int j = CHAR_BITS-1; j >= 0; j--){
    temp = (((sym) & (0x1 << (j))) >> (j));
    if(temporary == 1){
      buffer[buff_index / 8] = buffer[buff_index / 8] | (0x1 << (buff_index % 8));     
    }
    buff_index++;
  }
  buff_ind += CHAR_BITS / 8;
  return;
}

//
void flush_pairs(int outfile) {
  write(outfile, buffer, (buff_index / 8));
  memset(buffer, '\0', BUFF_LIMIT);
  buff_index = 0;
  buff_ind = 0;
  //bytes_out = bytes_out + ((buff_index / 8));
  return;
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen) {
  // Read a block of pairs.
  if((read_ind == 0) || (read_ind >= BUFF_LIMIT)){
    memset(in_buffer, '\0', BUFF_LIMIT);
    int bread = 1;
    int t_bread = 0;
    while((t_bread < BUFF_LIMIT) && (bread != 0)){
      bread = read(infile, in_buffer+t_bread, BUFF_LIMIT-t_bread);
      t_bread += bread;
    }
    read_ind = 0;
    read_index = 0;
  }

  uint16_t temp_code = 0;
  for (int i = bitlen-1; i >= 0; i--) {
    int binary = ((in_buffer[read_index / 8] & (0x1 << (read_index % 8))) >> (read_index % 8));
    if (binary == 1) {
      temp_code = temp_code | (0x1 << i);
    }
    read_index++;
  }
  read_ind += bitlen / 8;
  *code = temp_code;

  uint8_t temp_sym = 0;
  for(int j = CHAR_BITS-1; j >= 0; j--){
    int binary = ((in_buffer[read_index / 8] & (0x1 << (read_index % 8))) >> (read_index % 8));
    if (binary == 1) {
     temp_sym = temp_sym | (0x1 << j);
    }
    read_index++;
  }

  read_ind += CHAR_BITS / 8;
  *sym = temp_sym;

  if(*code == STOP_CODE){
    return false;
  }
  return true;
}

// Buffers a Word, or more specifically, the symbols of a Word.
// Each symbol of the Word is placed into a buffer.
// The buffer is written out when it is filled
/*void buffer_word(int outfile, Word *w) {
  int temporary;
  int byte_num = w->len;
  for (int i = 0; i < byte_num; i++) {
    for (int j = 0; j < CHAR_BITS; j++) {
      temporary = ((w->syms[i] & (0x1 << (j))) >> (j));//Keeps track if the bit is set.
      if (temporary == 1) {
        word_buff[word_buff_ind] = word_buff[word_buff_ind] | (0x1 << (j));//Put it in the buffer.
      }
    word_buff_index++;
    }
    word_buff_ind++;
    if (word_buff_ind == BUFF_LIMIT) {//If the buffer is full write out the buffer.
      write(outfile, word_buff, BUFF_LIMIT);
      word_buff_index = 0;
      //de_bytes_out = de_bytes_out + BUFF_LIMIT;
      word_buff_ind = 0;
    }
  }
  return;
}
*/
void buffer_word(int outfile, Word *w) {
  // Buffers a Word, or more specifically, the symbols of a Word.
  // Each symbol of the Word is placed into a buffer.
  // The buffer is written out when it is filled
  //word_buff
  int byte_num = w->len;

  for(int i = 0; i < byte_num; i++){
    for(int j = 0; j < CHAR_BITS; j++){
      int temp = ((w->syms[i] & (0x1 << (j))) >> (j));//Keeps track if the bit is set.
      if (temp == 1) {
        word_buff[word_buff_ind] = word_buff[word_buff_ind] | (0x1 << (j));//Put it in the buffer.
      }
    }
    word_buff_ind++;
    if(word_buff_ind == BUFF_LIMIT){
      write(outfile, word_buff, BUFF_LIMIT);
      memset(word_buff, '\0', BUFF_LIMIT);
      word_buff_ind = 0;
    } 
  }
  return;
}


// Writes out any remaining symbols in the buffer.
void flush_words(int outfile) {
  //de_bytes_out = de_bytes_out + word_buff_ind;
  write(outfile, word_buff, word_buff_ind);//If the buffer is not full but the end of the file has been reached, write out the amount of bits that the buffer has.
  memset(word_buff, '\0', BUFF_LIMIT);
  word_buff_ind = 0;
  return;
}
