#include "AES.h"
#include "byte_ops.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_USE "Usage: AES [-mode] [-option] [input file] [output file]\n\
    modes: [-e] encryption, [-d] decryption\n\
    options: [] stdin/stdout, [-f] seperate in/out files\n\
    Filenames must follow the file option in the order of input and then output.\n\
***Warning*** For the seperate files option, the ouput file will be overwritten if it already exists.\n"

#define BUF_LEN 1024 /* Size of buffer*/

static int nextBlock(FILE *fp, uint8_t dest[], size_t len);
static bool fileEncrypt(FILE *in, FILE *out, uint8_t key[]);
static bool fileDecrypt(FILE *in, FILE *out, size_t fileLen, uint8_t key[]);

static size_t readConsole(uint8_t **textBytes, Direction direction);
static void consoleEncrypt(uint8_t bytes[], size_t len, uint8_t key[]);
static bool consoleDecrypt(uint8_t bytes[], size_t len, uint8_t key[]);

static bool readKey(uint8_t key[], size_t len);
static void errorExit(char *msg, int numFiles, FILE *fpv[]);

int main(int argc, char *argv[])
{
    if (argc > 5)
    {
        errorExit(ERROR_USE, 0, NULL);
    }

    Direction direction;
    bool notSet = true; //mode not choosen yet
    FILE *in;
    FILE *out;
    char *inFile = NULL;
    char *outFile;
    /* Validating command line options and file names */
    for (int i = 1; i < argc; i++)
    {
        if (notSet && (strcmp(argv[i], "-e") == 0))
        {
            direction = ENCRYPT;
            notSet = false;
        }
        else if (notSet && (strcmp(argv[i], "-d") == 0))
        {
            direction = DECRYPT;
            notSet = false;
        }
        else if ((i < argc - 2) && (strcmp(argv[i], "-f") == 0))
        {
            inFile = argv[++i];
            outFile = argv[++i];
        }
        else
        {
            errorExit(ERROR_USE, 0, NULL);
        }
    }
    if (notSet)
    {
        errorExit(ERROR_USE, 0, NULL);
    }

    uint8_t keyBytes[BLOCK_SIZE];
    if (!readKey(keyBytes, BLOCK_SIZE))
    {
        errorExit("Invalid key input", 0, NULL);
    }

    /* process the input either from console or file */
    if (inFile == NULL)
    {
        uint8_t *textBytes = NULL;
        size_t lenBytes = readConsole(&textBytes, direction);
        if (lenBytes == 0)
        {
            errorExit("Error converting text to bytes", 0, NULL);
        }
        else if (direction == ENCRYPT)
        {
            consoleEncrypt(textBytes, lenBytes, keyBytes);
        }
        else if (!consoleDecrypt(textBytes, lenBytes, keyBytes))
        {
            errorExit("Invalid ciphertext. Could not decrypt", 0, NULL);
        }
        free(textBytes);
    }
    else
    {
        if ((in = fopen(inFile, "rb")) == NULL)
        {
            errorExit(strcat("Error opening input file: ", inFile), 0, NULL);
        }

        if ((out = fopen(outFile, "wb")) == NULL)
        {
            errorExit(strcat("Error opening output file: ", outFile), 1, (FILE *[]){in});
        }

        fseek(in, 0L, SEEK_END);
        size_t fileLen = ftell(in);
        rewind(in);

        if (direction == ENCRYPT && !fileEncrypt(in, out, keyBytes))
        {
            errorExit("Error reading input file", 2, (FILE *[]){in, out});
        }
        else if (direction == DECRYPT && !fileDecrypt(in, out, fileLen, keyBytes))
        {
            errorExit("Error reading input file", 2, (FILE *[]){in, out});
        }
        fclose(in);
        fclose(out);
    }

    exit(EXIT_SUCCESS);
}

/**
 * Reads bytes from a file using a buffer. The buffer position indicates the next 
 * byte to be read. A position equal to the size indicates that all data has been read. 
 * Three cases when new data is read: 
 * 1. Full buffer 
 * 2. Partial buffer due to EOF
 * 3. Partial buffer due to error 
 * 
 * First two cases are handled similarly. Last case returns -1 as an error flag. 
*/
int nextBlock(FILE *fp, uint8_t dest[], size_t len)
{
    static uint8_t buf[BUF_LEN];
    static size_t bufs = 0;     // buffer size
    static size_t bufp = 0;     // buffer position

    /* read new data to buffer when no more unused data in the buffer*/
    if (bufp >= bufs)
    {
        bufs = fread(buf, sizeof(uint8_t), BUF_LEN, fp);
        bufp = 0;
        if (ferror(fp))
            return -1;
    }
    /* minimum of unused bytes available vs bytes requested  */
    size_t bytes = ((bufs - bufp) < len) ? (bufs - bufp) : len;

    copy(dest, 0, buf, bufp, bytes);
    bufp += bytes;
    return bytes;
}

bool fileEncrypt(FILE *in, FILE *out, uint8_t key[])
{
    uint8_t block[BLOCK_SIZE];
    int bytes = nextBlock(in, block, BLOCK_SIZE);
    while ((bytes >= 0) && ((size_t)bytes == BLOCK_SIZE))
    {
        encrypt(block, key);
        fwrite(block, sizeof(uint8_t), BLOCK_SIZE, out);
        bytes = nextBlock(in, block, BLOCK_SIZE);
    }
    // encrypt last block, pad as necessary using PKCS7
    if (bytes >= 0)
    {
        pad(block, bytes);
        encrypt(block, key);
        fwrite(block, sizeof(uint8_t), BLOCK_SIZE, out);
        return true;
    }
    // there was a read error before EOF
    return false;
}

bool fileDecrypt(FILE *in, FILE *out, size_t fileLen, uint8_t key[])
{
    uint8_t block[BLOCK_SIZE];
    int bytes = nextBlock(in, block, BLOCK_SIZE);
    for (unsigned i = 1; (bytes > 0) && ((size_t)bytes == BLOCK_SIZE); i++)
    {
        decrypt(block, key);
        if (i >= (fileLen / BLOCK_SIZE))
        {
            /**
             *  On the last block, remove padding. If the padding is invalid, 
             *  this still decrypts but the result is gibberish. 
             */
            fwrite(block, sizeof(uint8_t), (BLOCK_SIZE - checkPad(block)), out);
            return true;
        }
        fwrite(block, sizeof(uint8_t), BLOCK_SIZE, out);
        bytes = nextBlock(in, block, BLOCK_SIZE);
    }
    // there was a read error before EOF
    return false;
}

/**
 * Allows the user to quickly encrypt and decrypt a single string by reading from standard input 
 * instead of a file. The input is first stored as a text string, then it is converted to bytes. 
*/
size_t readConsole(uint8_t **bytesPtr, Direction direction)
{
    printf("You can enter up to %d characters, including the newline, to be processed.\n"
           "Ciphertext for decryption must be a string of hexadecimal bytes, two digits"
           "each, with no space in between bytes\n",
           BUF_LEN - 1);
    printf("Enter text: ");
    char temp[BUF_LEN];
    char *textPtr = fgets(temp, BUF_LEN, stdin);
    if (textPtr == NULL)
    {
        return 0;
    }
    size_t lenText = strlen(textPtr);
    if (textPtr[lenText - 1] == '\n')
    {
        textPtr[lenText - 1] = '\0';
        lenText--;
    }
    printf("Read following text: ");
    puts(textPtr);

    /**
     * If encrypting, each character in input is a byte. If decrypting, every
     * two characters in input form a hexademical numeral that represents a byte. 
    */
    size_t lenBytes = (direction == ENCRYPT) ? lenText : lenText / 2;
    if ((*bytesPtr = (uint8_t *)calloc(lenBytes, sizeof(uint8_t))) == NULL)
    {
        return 0;
    }

    if (direction == ENCRYPT)
    {
        copy(*bytesPtr, 0, (uint8_t *)textPtr, 0, lenBytes);
    }
    else if (!xStrToBytes(textPtr, lenText, *bytesPtr))
    {
        return 0;
    }
    return lenBytes;
}

void consoleEncrypt(uint8_t bytes[], size_t len, uint8_t key[])
{
    uint8_t block[BLOCK_SIZE];
    printf("Encrypted text in hex: ");
    for (size_t i = 0; i < (len / BLOCK_SIZE); i++)
    {
        copy(block, 0, bytes, i * BLOCK_SIZE, BLOCK_SIZE);
        encrypt(block, key);
        printBytes(block, BLOCK_SIZE);
    }
    //pad the last block using PKCS7 
    size_t remaining = len % BLOCK_SIZE;
    copy(block, 0, bytes, (len / BLOCK_SIZE), remaining);
    pad(block, remaining);
    encrypt(block, key);
    printBytes(block, BLOCK_SIZE);
    printf("\n");
}

bool consoleDecrypt(uint8_t bytes[], size_t len, uint8_t key[])
{
    if (len % BLOCK_SIZE)
    {
        return false;
    }

    uint8_t block[BLOCK_SIZE];
    printf("Decrypted text: ");
    for (size_t i = 0; i < len / BLOCK_SIZE; i++)
    {
        copy(block, 0, bytes, i * BLOCK_SIZE, BLOCK_SIZE);
        decrypt(block, key);
        if (i == (len / BLOCK_SIZE - 1))
        {
            printBytes(block, BLOCK_SIZE - checkPad(block));
        }
        printBytes(block, BLOCK_SIZE);
    }
    return true;
}

bool readKey(uint8_t key[], size_t len)
{
    char buf[BUF_LEN];
    printf("Enter key: ");
    char *bufPtr = fgets(buf, BUF_LEN, stdin);
    char keyStr[len + 1];
    if (bufPtr == NULL || (sscanf(bufPtr, "%s", keyStr) != 1) || (strlen(keyStr) != len))
    {
        return false;
    }
    printf("Read following key: ");
    puts(keyStr);
    copy(key, 0, (uint8_t *)keyStr, 0, len);
    return true;
}

void errorExit(char *msg, int numFiles, FILE *fpv[])
{
    fprintf(stderr, "%s\n", msg);
    for (int i = 0; i < numFiles; i++)
    {
        fclose(fpv[i]);
    }
    exit(EXIT_FAILURE);
}