/* Implementation of SHA-256 by PK22.
   Not the best quality; written by adapting Wikipedia's pseudocode implementation.
   Even though I mainly code in C++, I might have cases where I need to use a hash function in C.

Exports four functions implementing SHA-256 (remember to free() each once done!):

sha256(const void* start, const size_t bytes)               -> uint8_t* to 32-byte array containing the digest.
sha256_str(const char* string)                              -> Same as above, but digests a (C) string instead.
sha256_hexdigest(const void* start, const size_t bytes)     -> char* to a 64-character C string containing the hexdigest of the hash.
sha256_str_hexdigest(const char* string)                    -> Same as above, but for the hexdigest of a (C) string.

I don't advise using these functions for cryptography - I have no clue how to make an implementation secure.


Since this is probably going to be reused across multiple projects, might as well add a license:

Copyright (c) 2026 PK22

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Basic Chunk struct storing 64 32-bit words:
struct Chunk {
    uint32_t words[64];
};
// Bitwise right rotation:
uint32_t rightrotate(const uint32_t value, const int bits) {
    return (value >> bits) | (value << (32 - bits));
}
// Endian stuff (SHA-256 requires big-endian numbers):
uint32_t make_be32(const uint8_t* bytes) {
    return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) | ((uint32_t)bytes[2] << 8) | ((uint32_t)bytes[3]);
}
void copy_be32_bytes(uint8_t* bytes, const uint32_t num) {
    bytes[0] = (uint8_t)(num >> 24);
    bytes[1] = (uint8_t)(num >> 16);
    bytes[2] = (uint8_t)(num >> 8);
    bytes[3] = (uint8_t)(num);
}
void write_be64(uint8_t* bytes, const uint64_t num) {
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t)(num >> ((7-i) * 8));
    }
}
// Main internal function:
uint8_t* _hash_bytes(const void* start, const size_t bytes) {
    // Initialise hash values:
    uint32_t harray[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    // Initialise array of round constants:
    const uint32_t k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    size_t i;
    // Preprocessing: Padding
    const size_t bitlength = bytes * 8;
    
    //Find a value for K and L:
    const unsigned short K = 512 - ((bitlength + 1 + 64) % 512);
    const uint64_t L = bitlength;

    // Work out the new input sizes:
    const size_t totalinputlength = bitlength + 65 + K;
    const size_t inputbytes = totalinputlength / 8;
    
    // malloc() an array to store the new input:
    uint8_t* newinputarray = (uint8_t*)malloc(inputbytes);

    // Start filling in the data:
    memcpy(newinputarray, start, bytes);

    // Start with a 1, then pad with K zeroes:
    newinputarray[bytes] = 0x80;
    for (i = 1; i <= (K - 7) / 8; i++) {
        newinputarray[bytes + i] = 0x00;
    }

    // Finally, add the length of the original message as a 64-bit integer (copy 8 bytes):
    write_be64(newinputarray + inputbytes - 8, L);
    // Split into 512-bit chunks:
    const size_t numchunks = totalinputlength / 512;

    size_t c;
    for (c = 0; c < numchunks; c++) {
        uint8_t* chunkstart = newinputarray + (64 * c);
        struct Chunk chunk;
        // Copy into the first 16 words of the chunk;
        for (i = 0; i < 16; i++) {
            chunk.words[i] = make_be32(chunkstart + 4 * i);
        }
        for (i = 16; i < 64; i++) {
            uint32_t s0 = (rightrotate(chunk.words[i-15], 7)) ^ (rightrotate(chunk.words[i-15], 18)) ^ (chunk.words[i-15] >> 3);
            uint32_t s1 = (rightrotate(chunk.words[i-2], 17)) ^ (rightrotate(chunk.words[i-2], 19)) ^ (chunk.words[i-2] >> 10);
            chunk.words[i] = chunk.words[i-16] + s0 + chunk.words[i-7] + s1;
        }
        uint32_t a = harray[0];
        uint32_t b = harray[1];
        uint32_t c = harray[2];
        uint32_t d = harray[3];
        uint32_t e = harray[4];
        uint32_t f = harray[5];
        uint32_t g = harray[6];
        uint32_t h = harray[7];
        for (i = 0; i < 64; i++) {
            uint32_t S1 = (rightrotate(e, 6)) ^ (rightrotate(e, 11)) ^ (rightrotate(e, 25));
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + k[i] + chunk.words[i];
            uint32_t S0 = (rightrotate(a, 2)) ^ (rightrotate(a, 13)) ^ (rightrotate(a, 22));
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        harray[0] += a;
        harray[1] += b;
        harray[2] += c;
        harray[3] += d;
        harray[4] += e;
        harray[5] += f;
        harray[6] += g;
        harray[7] += h;
    }
    // Append values to produce final digest:
    uint8_t* digest = (uint8_t*)malloc(32);
    for (i = 0; i < 8; i++) {
        copy_be32_bytes(digest + 4 * i, harray[i]);
    }
    // Free the malloc()-ed new input array and return:
    free(newinputarray);
    return digest;
}

#ifdef __cplusplus
extern "C" {
#endif

// Exposed hash function for arbitrary bytes:
uint8_t* sha256(const void* start, const size_t bytes) {
    return _hash_bytes(start, bytes);
}

// Exposed function for strings:
uint8_t* sha256_str(const char* string) {
    return sha256(string, strlen(string));
}

// Hexdigest:
char* sha256_hexdigest(const void* start, const size_t bytes) {
    uint8_t* digest = _hash_bytes(start, bytes);
    // 65 character string for output:
    char* hexdigest = (char*)calloc(65, sizeof(char));
    // Ensure null-termination:
    hexdigest[64] = 0;
    const char hexchars[17] = "0123456789abcdef";
    for (int i = 0; i < 32; i++) {
        uint8_t cbyte = digest[i];
        hexdigest[2*i] = hexchars[(int)(cbyte / 16)];
        hexdigest[2*i+1] = hexchars[cbyte % 16];
    }
    free(digest);
    return hexdigest;
}

// Hexdigest of another string:
char* sha256_str_hexdigest(const char* string) {
    return sha256_hexdigest(string, strlen(string));
}
// Closing bracket for the earlier 'extern "C" {':
#ifdef __cplusplus
}
#endif
