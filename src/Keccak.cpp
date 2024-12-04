#include "Keccak.h"

const uint64_t Keccak::keccakf_rndc[24] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808A,
    0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008A,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
    0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
    0x000000000000800A, 0x800000008000000A, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

const int Keccak::keccakf_rotc[24] = {
    1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14, 
    27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

const int Keccak::keccakf_piln[24] = {
    10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4, 
    15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1 
};

Keccak::Keccak() {
    reset();
}

void Keccak::reset() {
    memset(state, 0, sizeof(state));
    byteIndex = 0;
}

void Keccak::add(const uint8_t *data, size_t length) {
    for(size_t i = 0; i < length; i++) {
        state[byteIndex++] ^= data[i];
        if(byteIndex >= 136) {
            keccakf();
            byteIndex = 0;
        }
    }
}

void Keccak::finalize(uint8_t *hash) {
    state[byteIndex] ^= 0x01;
    state[135] ^= 0x80;
    keccakf();
    
    for(int i = 0; i < 32; i++) {
        hash[i] = state[i];
    }
}

void Keccak::keccakf() {
    uint64_t *s = (uint64_t*)state;
    uint64_t t[5], c[5], d;
    
    for(int i = 0; i < 24; i++) {
        // Theta
        for(int x = 0; x < 5; x++) {
            c[x] = s[x] ^ s[x+5] ^ s[x+10] ^ s[x+15] ^ s[x+20];
        }
        
        for(int x = 0; x < 5; x++) {
            d = c[(x+4)%5] ^ ((c[(x+1)%5] << 1) | (c[(x+1)%5] >> 63));
            for(int y = 0; y < 25; y += 5) {
                s[y+x] ^= d;
            }
        }
        
        // Rho Pi
        d = s[1];
        for(int x = 0; x < 24; x++) {
            int j = keccakf_piln[x];
            uint64_t t = s[j];
            s[j] = ((d << keccakf_rotc[x]) | (d >> (64-keccakf_rotc[x])));
            d = t;
        }
        
        // Chi
        for(int y = 0; y < 25; y += 5) {
            for(int x = 0; x < 5; x++) {
                t[x] = s[y+x];
            }
            for(int x = 0; x < 5; x++) {
                s[y+x] ^= (~t[(x+1)%5]) & t[(x+2)%5];
            }
        }
        
        // Iota
        s[0] ^= keccakf_rndc[i];
    }
}