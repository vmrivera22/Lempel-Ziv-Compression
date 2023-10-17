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

uint64_t bytes_in = 0;
uint64_t bytes_out = 0;

uint8_t out_buff[BUFF_LIMIT+1];
uint32_t out_buff_ind = 0;

uint8_t in_buff[BUFF_LIMIT+1];
uint32_t in_buff_ind = 0;

// Function reads the FileHeader of a infile.
void read_header(int infile, FileHeader *header) {
  bytes_in = read_buffer(header, sizeof(FileHeader), infile);

  // If the machine is a Big Endian machine make the FileHeader Big Endian.
  if(is_big()){
    uint16_t temp_pro = swap16(header->protection);
    uint32_t temp_mag = swap32(header->magic);
    header->protection = temp_pro;
    header->magic = temp_mag;
  }
  return;
}

// Function writes the FileHeader to a outfile.
void write_header(int outfile, FileHeader *header) {
  // If the machine is a Big Endian machine make the FileHeader little Endian.
  if(is_big()){
    uint16_t temp_pro = swap16(header->protection);
    uint32_t temp_mag = swap32(header->magic);
    header->protection = temp_pro;
    header->magic = temp_mag;
  }

  // Write a block size of FileHeader or until there is an error writting.
  bytes_out = write_buffer(header, sizeof(FileHeader), outfile);
  
  return;
}

// Function reads a symbol from the infile and returns true if there are symbols to be read, otherwise returns false.
bool read_sym(int infile, uint8_t *sym) {
  // If we have not read any blocks or if we have processed all the symbols in the buffer read another block from the infile.
  if (in_buff_ind == 0 || in_buff_ind == BUFF_LIMIT) {
    // Read a block the size of a BUFF_LIMIT or until we reach the end of the file.
    bytes_in += read_buffer(in_buff, BUFF_LIMIT, infile);
    in_buff_ind = 0; // Reset the buffer index.
  }

  *sym = in_buff[in_buff_ind]; // Set sym to a symbol from the buffer that was read in.

  if (*sym == 0) {
    return false; // Return false if there is nothing left to read return false.
  }
  in_buff_ind++;
  return true;
}

// Function buffers pairs (code and symbol) and writes them to outfile when the buffer is full. 
void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bitlen) {
  // Process the input code.
  for (int i = 0; i < bitlen; i++) {
    // If the Buffer is full then write it to the outfile (done here since code is variable length).
    if((out_buff_ind) == (BUFF_LIMIT*8)){
      bytes_out += write_buffer(out_buff, BUFF_LIMIT, outfile);
      out_buff_ind = 0;
    }
    uint16_t temp = (((code) & (0x1 << (i))) >> (i)); // Check to see if the bit at i is set in code.
    if(temp == 1){ // If the bit is set then set it in the buffer at index out_buff_ind/8.
       out_buff[out_buff_ind / 8] = out_buff[out_buff_ind / 8] | (0x1 << (out_buff_ind % 8));
    }
    out_buff_ind++; // Keeps track of the bits that have been processed for current block.
  }

  // Process the input sym.
  for (int j = 0; j < CHAR_BITS; j++) {
    // If the Buffer is full then write it to the outfile (done here since code is variable length).
    if((out_buff_ind) == (BUFF_LIMIT*8)){
      bytes_out += write_buffer(out_buff, BUFF_LIMIT, outfile);
      out_buff_ind = 0;
    }
    uint16_t temp = (((sym) & (0x1 << (j))) >> (j)); // Check to see if the bit at j is set in sym.
    if(temp == 1){ // If the bit is set then set it in the buffer at index out_buff_ind/8.
      out_buff[out_buff_ind / 8] = out_buff[out_buff_ind / 8] | (0x1 << (out_buff_ind % 8));     
    }
    out_buff_ind++;
  }
  return;
}

// Function writes any remaining pairs to the outfile.
void flush_pairs(int outfile) {
  bytes_out += write_buffer(out_buff, (out_buff_ind / 8), outfile);
  out_buff_ind = 0;
  return;
}

// Function reads a pair from the infile and returns true if there are any pairs left to read, false otherwise.
bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bitlen) {
  uint16_t temp_code = 0; // Code to be copied from buffer.
  for (int i = 0; i < bitlen; i++) {
    // If in_buff_ind has reached the end of the buffer then read a new block (done here since code is variable length).
    if((in_buff_ind == 0) || (in_buff_ind >=  (BUFF_LIMIT*8))){
      bytes_in += read_buffer(in_buff, BUFF_LIMIT, infile); // Read a block the size of BUFF_LIMIT or until we reach the end of the file.
      in_buff_ind = 0;
    }
    
    int binary = ((in_buff[in_buff_ind / 8] & (0x1 << (in_buff_ind % 8))) >> (in_buff_ind % 8));  // Check to see if the bit at in_buff_ind%8 is set in buffer.
    if (binary == 1) { // If the bit is set then set bit i of temp_code.
      temp_code = temp_code | (0x1 << i);
    }
    in_buff_ind++;
  }
  *code = temp_code;

  uint8_t temp_sym = 0; // Sym to be copied from buffer
  for (int j = 0; j < CHAR_BITS; j++) {
    // If in_buff_ind has reached the end of the buffer then read a new block (done here since code is variable length).
    if((in_buff_ind == 0) || (in_buff_ind >=  (BUFF_LIMIT*8))){
      bytes_in += read_buffer(in_buff, BUFF_LIMIT, infile); // Read a block the size of BUFF_LIMIT or until we reach the end of the file.
      in_buff_ind = 0;
    }
    int binary = ((in_buff[in_buff_ind / 8] & (0x1 << (in_buff_ind % 8))) >> (in_buff_ind % 8)); // Check to see if the bit at in_buff_ind%8 is set in buffer.
    if (binary == 1) { // If the bit is set then set bit j of temp_sym.
     temp_sym = temp_sym | (0x1 << j);
    }
    in_buff_ind++;
  }
  *sym = temp_sym;

  if (*code == STOP_CODE) {
    return false;//If there is no more to read then return false.
  }
  return true;
}


// Function buffers words and writes them to outfile when the buffer is full.
void buffer_word(int outfile, Word *w) {
  // Buffer the sym to the buffer.
  for (int i = 0; i < w->len; i++) { // Process all characters of sym.
    for (int j = 0; j < CHAR_BITS; j++) {
      int temp = ((w->syms[i] & (0x1 << (j))) >> (j)); // Check if the bit j of the byte i (of sym) is set.
      if (temp == 1) { // If the bit in the char of sym is set then set the corresponding bit of the buffer.
        out_buff[out_buff_ind] = out_buff[out_buff_ind] | (0x1 << (j));
      }
    }
    out_buff_ind++;
    if (out_buff_ind == BUFF_LIMIT) { // If the buffer is full write out the buffer.
      bytes_out += write_buffer(out_buff, BUFF_LIMIT, outfile);
      out_buff_ind = 0;
    }
  }
  return;
}

// Function writes any remaining symbols in the buffer.
void flush_words(int outfile) {
  bytes_out += write_buffer(out_buff, out_buff_ind, outfile);
  out_buff_ind = 0;
  return;
}

// Function writes byte_num bytes from buffer to outfile.
int write_buffer(void *buffer, int byte_num, int outfile){
  int t_bwrite = 0;
  int check = 1;
  while((t_bwrite < byte_num) && (check > 0)){
    check = write(outfile, buffer+t_bwrite, byte_num-t_bwrite);
    t_bwrite += check;
  }
  if(check < 0){
    fprintf(stderr, "%s\n", strerror(errno));
  }
  memset(buffer, '\0', byte_num);
  return t_bwrite;
}

// Function reads byte_num bytes from infile to buffer.
int read_buffer(void *buffer, int byte_num, int infile) {
  memset(buffer, '\0', byte_num);
  int t_bread = 0;
  int check = 1;
  while((t_bread < byte_num) && (check > 0)){
    check = read(infile, buffer+t_bread, byte_num-t_bread);
    t_bread += check;
  }
  if(check < 0){
    fprintf(stderr, "%s\n", strerror(errno));
  }
  return t_bread;
}
