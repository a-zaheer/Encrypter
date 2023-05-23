#include "byte_ops.h"
#include <stdio.h>
#include <ctype.h>


static uint8_t hexToByte(char A, char B);

/**
 * Rotates the byte array as if it were a circular array. The bytes are shifted left by the specified number of indices.
 *
 * bytes: byte array
 * length: array length
 * shifts: how much to shift
 *
 * returns: true on success 
 *          false on error
 */
bool rotateLeft(uint8_t bytes[], size_t length, size_t shifts)
{
    uint8_t *temp = (uint8_t *)malloc(length * sizeof(uint8_t));
    if (temp == NULL)
        return false;

    for (size_t i = 0; i < length; i++)
    {
        temp[i] = bytes[(i + shifts) % length];
    }
    for (size_t i = 0; i < length; i++)
    {
        bytes[i] = temp[i];
    }
    free(temp);
    return true;
}

uint8_t * slice(uint8_t array[], size_t start, size_t end, size_t step)
{
    size_t length = ((end - start) / step) + 1;

    uint8_t *newArray = (uint8_t *)calloc(length, sizeof(uint8_t));
    for (size_t i = 0; i < length; i++)
    {
        newArray[i] = array[(i * step) + start];
    }
    return newArray;
}

void copy(uint8_t dest[], size_t d_off, uint8_t source[], size_t s_off, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        dest[i + d_off] = source[i + s_off];
    }
}

void transpose(uint8_t source[], uint8_t dest[], size_t m, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            dest[(i * m) + j] = source[(j * n) + i];
        }
    }
}

void printBytes(uint8_t bytes[], size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%02x", bytes[i]);
    }
}

bool xStrToBytes(char *str, size_t strLen, uint8_t bytes[])
{
    if (strLen % 2)
        return false; 
    
    for (size_t i = 0; i < strLen / 2; i++)
    {
        char a = str[2 * i];
        char b = str[(2 * i) + 1];
        if (!isxdigit(a) || !isxdigit(b))
        {
            return false;
        }
        bytes[i] = hexToByte(a, b);
    }
    return true;
}

static uint8_t hexToByte(char A, char B)
{
    char a = tolower(A);
    char b = tolower(B);
    uint8_t result;

    if (isalpha(a))
        result = (a - 'a' + 0xa);
    else
        result = (a - '0');
    result = result * 0x10;
    if (isalpha(b))
        result += (b - 'a' + 0xa);
    else
        result += (b - '0');

    return result;
}

uint8_t xtime(uint8_t a) {
    uint16_t result = (uint16_t) a; 
    result = result << 1; 
    if (result > UINT8_MAX) //the degree of polynomial is 8 or higher 
        result = result ^ FIELD_POLY; 
    return (uint8_t) result; 
}

uint8_t * arrXor(uint8_t a[], uint8_t b[], size_t length) {
    for(size_t i = 0; i < length; i++) {
        a[i] = a[i] ^ b[i]; 
    } 
    return a; 
}

uint8_t dot(uint8_t a[], uint8_t b[], size_t length) {
    uint8_t result = 0; 
    for(size_t i = 0; i < length; i++) {
        result = result ^ (multiply(a[i], b[i])); 
    }
    return result; 
}

uint8_t multiply(uint8_t a, uint8_t b) {
    uint8_t poly1 = a; 
    uint8_t temp = b; 
    uint8_t result = 0; 
    while(poly1 > 0) {
        if(poly1 % 2) 
            result = result ^ temp; 
        temp = xtime(temp); 
        poly1 /= 2; 
    }
    return result; 
}
