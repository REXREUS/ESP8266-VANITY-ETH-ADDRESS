#ifndef Keccak_h
#define Keccak_h

#include <stdint.h>
#include <cstddef>  // Untuk size_t
#include <cstring>  // Untuk memset

class Keccak {
public:
    Keccak();
    void reset();
    void add(const uint8_t *data, std::size_t length);
    void finalize(uint8_t *hash);
    
private:
    void keccakf();
    uint8_t state[200];
    std::size_t byteIndex;
    static const uint64_t keccakf_rndc[24];
    static const int keccakf_rotc[24];
    static const int keccakf_piln[24];
};

#endif