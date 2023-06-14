# Encrypter
**FOR LEARNING PURPOSES ONLY,  DO NOT USE THIS PROGRAM TO ENCRYPT SENSITIVE DATA**
## What Does It Do?
Encrypter can encrypt files using the Advanced Encryption Standard (AES), more specifically AES-128, as defined by the National Institute of Standards and Technology (NIST). It can also encrypt a string of text that is inputted through the command-line terminal. The program can decrypt ciphertext that it originally encrypted for both modes of input. In this document, plaintext and ciphertext refer to the bytes before and after encryption respectively, regardless of whether they represent text or binary data. 

Files are completely encrypted such that every byte of plaintext gets transformed into ciphertext. The encryption is performed one block of bytes at a time using the Electronic Code Book (ECB) mode. The final block is padded using PKCS#7 padding before being encrypted. After decryption, the padding is removed so that the user only deals with the original plain text. 

## Getting Started 
Windows: You will need Visual Studio installed for the C compiler. Open the Developer Command Prompt and navigate to the local directory containing the source code files. Next, compile the files into an executable using the command "cl AES.c byte_ops.c runner.c" /link /out:<executable-name>.exe".

Linux: You will need the GNU C compiler installed. Open up the terminal and navigate to the local directory containing the source code files. Next, compile the source files using the command "gcc -o <executable-name> -std=c11 -AES.c -byte_ops.c -runner.c". 

## How to Run 
In our case, we named the executable "AES". Navigate to the directory containing the executable and run the following command "AES [-mode] [-option] <input file> <output file>" in the terminal. 
  
Mode: Use '-e' to encrypt and '-d' to decrypt
  
Option: Leave it blank to simply encrypt a string through the terminal or use '-f' to encrypt a file. 
  
Files: If the file option is selected, then enter the relative path of the input file and the relative path of where you want the output file to go. If the input file is in the same directory and you want the output file to be there as well, then just use their names. 
  
Example: To encrypt a file in the current directory I will input "AES -e -f exampleFile.txt outFile.txt.enc". This takes a file called "exampleFile.txt" and outputs an encrypted file named "outFile.txt.enc". The custom file extension is simply there to help with recovering the original extension when decrypting.
  
The program provides instructions for the rest of the steps. 

## AES Background 
### AES-128
AES is a block cipher. That means it takes a block of bytes, performs a series of reversible transformation rounds, and then outputs another block of bytes. Both the input and output blocks consist of 16 bytes or 128 bits. It also takes in a key, which is 16 bytes for AES-128, to encrypt the plaintext. Using that same key, AES can decrypt the ciphertext by reversing all the transformations. 

The plaintext bytes are stored in a 4 x 4 state array which is filled one column at a time. The initial 16-byte key is expanded to 176 bytes using the standard AES key schedule to provide 11 separate blocks of 16-byte keys. The key bytes are likewise stored in 4 x 4 arrays. The state array is combined with the first key array using bitwise XOR on the corresponding bytes. Then, the state array goes through 9 transformation rounds, where each round consists of: 
1. SubBytes: Each byte is substituted using a predefined lookup table 
2. ShiftRows: The rows of the state array are cyclically rotated to the left with an increasing number of steps. For instance, the first row is unchanged and the bytes in the fourth row are shifted three places left. 
3. MixColumns: An invertible linear transformation is applied to each column of the state array. Each column is multiplied by a predefined matrix to achieve the transformed column. All arithmetic is done in the GF(2^8) finite field. 
4. AddRoundKey: The state array is combined with a new key array using bitwise XOR like the initial step. 

Finally, the 10th round omits the MixColumns step such that it only consists of: 
1. SubBytes
2. ShiftRows
3. AddRoundKey

The final state array holds the ciphertext. Decryption follows the same process in reverse. The key is expanded using the same key schedule at the start, and then the key blocks are used starting from the end. Each step of the encryption transformation round is an invertible calculation or has a corresponding reverse lookup table. 
### ECB
The AES algorithm only works on a single block of 16 bytes at a time, so multiple protocols can be used to encrypt plaintext consisting of multiple blocks. The simplest method is Electronic Code Book (ECB) mode. In ECB, the user provides a single 128-bit key, the plaintext is broken into blocks of 16 bytes, and each block is encrypted independently with AES using the single key. Two plaintext blocks that are the same will get encrypted to the same ciphertext which may leak information. For instance, if each pixel of an image corresponds to a single block, then all the same colors on an image will get encrypted the same. Patterns in the original image will persist in the ciphertext. ECB is not very secure, but it is simple to implement and fast to execute. 
### PKCS#7
Since AES works in blocks of 16 bytes, filler bytes may need to be appended to the plaintext to extend its length to a multiple of 16. After the original plaintext is split up, the final block can have anywhere from 1 - 16 bytes. PKCS#7 pads this final block based on how many bytes it has.
1. If it has 1 - 15 bytes left, it uses the number of free spots as the padding. For instance, if the block had one byte of arbitrary data, 'XX' in hex, then it would pad the remaining 16 - 1 positions with '0F' which is 15 in hex. XX -> XX0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F
2. If it has 16 bytes, then an additional block of 16 '00's is appended to the plaintext. 

Even when the plaintext is a multiple of 16, padding is added so that the final byte in the final block is guaranteed to be a padding byte. This makes it possible to recover the original plaintext after decryption. 
