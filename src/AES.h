/**
 * @file AES.h
 *
 * @brief constants and function prototypes for AES.c
 *
 * Holds the Direction enum which is supplied to the program's functions to
 * indicate encryption or decryption. Additionally, holds the AES algorithm
 * constants and function prototypes.
 *
 * @author Ali Zaheer
 */

#ifndef AES_H
#define AES_H

/* -- Includes -- */
#include <stdint.h>
#include <stdlib.h>

typedef enum { ENCRYPT, DECRYPT } Direction;

extern const size_t WORD_SIZE;
extern const size_t BLOCK_SIZE; /* 16 bytes */
extern const size_t KEY_SIZE;   /* 128 bit - 16 bytes */
extern const int NUM_ROUNDS;

/**
 * @brief Encrypts a block of bytes using AES-128 and the given key.
 *
 * Both the key and the data block should be 16 bytes long.
 *
 * @param block The block of bytes to be encrypted
 * @param key The key
 *
 * @return The encrypted block
 */
uint8_t *encrypt(uint8_t block[], uint8_t key[]);

/**
 * @brief Decrypts a block of bytes encrypted using this program and the given
 * key.
 *
 * Both the key and the data block should be 16 bytes long.
 *
 * @param block The encrypted block of bytes to be decrypted
 * @param key The key used during encryption
 *
 * @return The decrypted block
 */
uint8_t *decrypt(uint8_t block[], uint8_t key[]);

/**
 * @brief Checks if an block has a valid PKCS7 padding.
 *
 * @param block The block of bytes to check
 *
 * @return The number of padding bytes, or 0 if invalid padding.
 */
uint8_t checkPad(uint8_t block[]);

/**
 * @brief Pads a block of bytes using PKCS7
 *
 * @param block The block of bytes to be padded
 * @param len The length of the block
 */
void pad(uint8_t block[], size_t len);

#endif