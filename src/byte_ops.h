/**
 * @file byte_ops.h
 * @brief Provides helper function prototypes and defines the field polynomial
 *
 * The helper functions declared here are used in encryption(AES.c) and
 * input/output(runner.c). FILED_POLYNOMIAL is the binary representation of the
 * polynomial used to define the AES field GF(2^8) .
 *
 * @author Ali Zaheer
 */

#ifndef BYTE_OPS_H
#define BYTE_OPS_H

/* -- Includes -- */
#include "AES.h" /* for Direction */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#undef FIELD_POLY
#define FIELD_POLY 0b100011011 /* AES polynomial */

/**
 * @brief Creates a sub-array from an array
 *
 * Input rules:
 * 1. start <= end < length of array
 * 2. step <= (end - start)
 *
 * The sub-array is dynamically allocated, so the caller must free it once done.
 *
 * @param array The original array of bytes
 * @param start Sub-array's start in array
 * @param end Sub-array's end in array
 * @param step Increment of elements to be copied
 *
 * @return Pointer to the new sub-array
 */
uint8_t *slice(uint8_t array[], size_t start, size_t end, size_t step);

/**
 * @brief Copies bytes from one array to another
 *
 * Input rules:
 * 1. Index parameters must be valid for their respective arrays
 * 2. length <= min(length of dest - d, length of source - s)
 *
 * @param dest Destination array
 * @param d Starting index in destination to copy into
 * @param source Source array
 * @param s Starting index in source to copy from
 * @param length Number of bytes to copy
 */
void copy(uint8_t dest[], size_t d, uint8_t source[], size_t s, size_t length);

/**
 * @brief Transposes a matrix, represented by a linear array, and stores the
 * result in another array
 *
 * Both matrices will be stored in a single dimensional array, represented using
 * row-major order.
 *
 * @param Source Array representation of original matrix
 * @param Dest Array to host transposed matrix
 * @param m Number of rows in original matrix
 * @param n Number of columns in original matrix
 */
void transpose(uint8_t source[], uint8_t dest[], size_t m, size_t n);

/**
 * @brief Circularly shifts all the bytes in the array to the left by the
 * specified number of indices.
 *
 * The input array will be modified.
 *
 * @param bytes The array of bytes
 * @param length Array length
 * @param shifts Number of shifts to the left
 *
 * @return True if successful rotation, otherwise false
 */
bool rotateLeft(uint8_t bytes[], size_t length, size_t shifts);

/**
 * @brief Print all the bytes to a string.
 *
 * If encrypting, the bytes are printed to a hexadecimal string.Each byte is
 * represented by a two digit hexadecimal numeral. If decrypting
 * the bytes are interpreted as ASCII/UNICODE characters.
 *
 * @param bytes The byte array
 * @param len Array length
 * @param direction Encrypting/decrypting
 */
void printBytes(uint8_t bytes[], size_t len, Direction direction);

/**
 * @brief Parses a hexadecimal string and converts to byte array
 *
 * The string must contain only hexadecimal characters, with exactly two digits
 * for each hexadecimal numeral. Each numeral is stored as byte in the array.
 * For instance '01A2B5FF' is stored as [0x01, 0xA2, 0xB5, 0xFF]. The byte array
 * must be big enough to store all the numerals in the string. All bytes will be
 * stored up to the point of error or completion.
 *
 * @param str The string
 * @param strLen String length
 * @param bytes Array to store result
 *
 * @return True if the string was parsed completely, false if error
 */
bool xStrToBytes(char *str, size_t strLen, uint8_t bytes[]);

/**
 * @brief Multiplies a polynomial by x^1 in GF(2^8).
 *
 * The polynomial p is represented by a byte, with the low order bits
 * corresponding to to the low degree terms. The function computes p * x using
 * the finite field's arithmetic.
 *
 * @param a The polynomial byte
 *
 * @return The product
 */
uint8_t xtime(uint8_t a);

/**
 * @brief Multiplies two polynomials in GF(2^8)
 *
 * Polynomials are represented by bytes, with the low order bits corresponding
 * to the low degree terms. The result is in GF(2^8).
 *
 * @param a First polynomial byte
 * @param b Second polynomial byte
 *
 * @return The product
 */
uint8_t multiply(uint8_t a, uint8_t b);

/**
 * @brief Applies XOR to corresponding elements of two byte arrays.
 *
 * The result will have elements a[i] = a[i] XOR b[i].
 * Length can't be greater than the length of the smaller array
 *
 * @param a First byte array
 * @param b Second byte array
 * @param length Number of elements to XOR
 *
 * @return Pointer to first array after XOR operations
 */
uint8_t *arrXor(uint8_t a[], uint8_t b[], size_t length);

/**
 * @brief Computes the dot product of two vectors in the vector space over
 * GF(2^8).
 *
 * The vectors are represented by byte arrays and all arithmetic is done in
 * GF(2^8). Length can't get greater than the length of the smaller array.
 *
 * @param a First vector array
 * @param b Second vector array
 * @param length Number of entries included in the dot product
 *
 * @return Dot product
 */
uint8_t dot(uint8_t a[], uint8_t b[], size_t length);

#endif