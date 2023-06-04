# Encrypter
**FOR LEARNING PURPOSES ONLY,  DO NOT USE THIS PROGRAM TO ENCRYPT SENSITIVE DATA**
## What Does It Do?
Encrypter can encrypt files using the Advanced Encryption Standard (AES), more specifically AES-128, as defined by the National Institute of Standards and Technology (NIST). It can also encrypt a string of text that is inputted through the command-line terminal. The program can decrypt ciphertext that it originally encrypted for both modes of input. In this document, plaintext and ciphertext refer to the bytes before and after encryption respectively, regardless of whether they represent text or binary data. 

Files are completely encrypted such that every byte of plaintext gets transformed into ciphertext. The encryption is performed one block of bytes at a time using the Electronic Code Book (ECB) mode. The final block is padded using PKCS#7 padding before being encrypted. After decryption, the padding is removed so that the user only deals with the original plain text. 

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
### PKCS#7
