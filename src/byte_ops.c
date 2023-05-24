/**
 * @file byte_ops.c
 * @brief Implements the supporting byte array operations and finite field
 * arithmetic.
 *
 * @author Ali Zaheer
 */

/* -- Includes -- */
#include "byte_ops.h" /* For public function prototypes */
#include <ctype.h>    /* For char functions */
#include <stdio.h>

/* Local function */
static uint8_t hexToByte(char A, char B);

uint8_t *slice(uint8_t array[], size_t start, size_t end, size_t step) {
  size_t length = ((end - start) / step) + 1;

  uint8_t *newArray = (uint8_t *)calloc(length, sizeof(uint8_t));
  for (size_t i = 0; i < length; i++) {
    newArray[i] = array[(i * step) + start];
  }
  return newArray;
}

void copy(uint8_t dest[], size_t d_off, uint8_t source[], size_t s_off,
          size_t length) {
  for (size_t i = 0; i < length; i++) {
    dest[i + d_off] = source[i + s_off];
  }
}

/* The transposed matrix will be n x m. It will have entries (i , j).
 * (i * m) + j is the j-th element in i-th row, where each row has m entries.
 */
void transpose(uint8_t source[], uint8_t dest[], size_t m, size_t n) {
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < m; j++) {
      dest[(i * m) + j] = source[(j * n) + i];
    }
  }
}

bool rotateLeft(uint8_t bytes[], size_t length, size_t shifts) {
  uint8_t *temp = (uint8_t *)malloc(length * sizeof(uint8_t));
  if (temp == NULL)
    return false;

  for (size_t i = 0; i < length; i++) {
    temp[i] = bytes[(i + shifts) % length];
  }
  for (size_t i = 0; i < length; i++) {
    bytes[i] = temp[i];
  }
  free(temp);
  return true;
}

void printBytes(uint8_t bytes[], size_t len, Direction direction) {
  if (direction == ENCRYPT) {
    for (size_t i = 0; i < len; i++) {
      printf("%02x", bytes[i]);
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      putchar(bytes[i]);
    }
  }
}

/* each hex numeral has to be exactly two digits long */
bool xStrToBytes(char *str, size_t strLen, uint8_t bytes[]) {
  if (strLen % 2)
    return false;

  for (size_t i = 0; i < strLen / 2; i++) {
    char a = str[2 * i];       /* high order hex digit */
    char b = str[(2 * i) + 1]; /* low order hex digit */
    if (!isxdigit(a) || !isxdigit(b)) {
      return false;
    }
    bytes[i] = hexToByte(a, b);
  }
  return true;
}

uint8_t xtime(uint8_t a) {
  uint16_t result = (uint16_t)a; /* allow for overflow */
  result = result << 1;          /* equivalent to multiplying by x^1 */
  if (result > UINT8_MAX)        /* if polynomial degree is >= 8, then reduce */
    result = result ^ FIELD_POLY;
  return (uint8_t)result; /* Cast doesn't change the value in GF(2^8)*/
}

/* shift-and-add multiplication */
uint8_t multiply(uint8_t a, uint8_t b) {
  uint8_t poly1 = a;
  uint8_t temp = b;
  uint8_t result = 0;
  while (poly1 > 0) {
    if (poly1 % 2)
      result = result ^ temp;
    temp = xtime(temp);
    poly1 /= 2;
  }
  return result;
}

uint8_t *arrXor(uint8_t a[], uint8_t b[], size_t length) {
  for (size_t i = 0; i < length; i++) {
    a[i] = a[i] ^ b[i];
  }
  return a;
}

uint8_t dot(uint8_t a[], uint8_t b[], size_t length) {
  uint8_t result = 0;
  for (size_t i = 0; i < length; i++) {
    result = result ^ (multiply(a[i], b[i]));
  }
  return result;
}

uint8_t hexToByte(char A, char B) {
  char a = tolower(A);
  char b = tolower(B);
  uint8_t result;

  if (isalpha(a))
    result = (a - 'a' + 0xa);
  else
    result = (a - '0');
  result = result * 0x10; /* high order digit */
  if (isalpha(b))
    result += (b - 'a' + 0xa);
  else
    result += (b - '0');

  return result;
}
