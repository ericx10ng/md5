#ifndef __MD5_H__
#define __MD5_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#define SINGLE_ONE_BIT 0x80
#define BLOCK_SIZE 512
#define MOD_SIZE 448
#define APP_SIZE 64
#define BITS 8
  // MD5 Chaining Variable
#define A 0x67452301UL
#define B 0xEFCDAB89UL
#define C 0x98BADCFEUL
#define D 0x10325476UL

int32_t md5_start (void);
int32_t md5_update (uint32_t *w, int len);
uint32_t md5_padding(uint8_t*data, uint32_t len, uint32_t msg_length);
int32_t md5_end (uint8_t *md5_32);
int md5_demo(char* data, int msg_len, uint8_t* result);

#endif
