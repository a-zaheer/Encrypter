#ifndef BYTE_OPS_H
#define BYTE_OPS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#undef FIELD_POLY
#define FIELD_POLY 0b100011011 /* AES field defining polynomial */

uint8_t * slice(uint8_t array[], size_t start, size_t end, size_t step);
void copy(uint8_t dest[], size_t d, uint8_t source[], size_t s, size_t length);
void transpose(uint8_t source[], uint8_t dest[], size_t m, size_t n);
bool rotateLeft(uint8_t bytes[], size_t length, size_t shifts);
void printBytes(uint8_t bytes[], size_t len);

bool xStrToBytes(char *str, size_t strLen, uint8_t bytes[]);

uint8_t xtime(uint8_t a);
uint8_t multiply(uint8_t a, uint8_t b);
uint8_t * arrXor(uint8_t a[], uint8_t b[], size_t length);
uint8_t dot(uint8_t a[], uint8_t b[], size_t length); 

#endif