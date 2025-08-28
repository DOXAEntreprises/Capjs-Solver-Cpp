#include "../../include/cap_c_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
    // Test single challenge
    uint64_t nonce = cap_solve_pow("test", "00");
    if (nonce != UINT64_MAX) {
        printf("nonce: %llu\n", nonce);
    } else {
        return 1;
    }
    
    // Test PRNG
    char prng_output[17]; // 16 chars + null terminator
    if (cap_prng_generate("test", 16, prng_output) == 0) {
        printf("prng: %s\n", prng_output);
    } else {
        return 1;
    }
    
    // Test multiple challenges
    const size_t count = 3;
    uint64_t results[3];
    
    if (cap_solve_challenges("test", count, 8, 1, results) == 0) {
        printf("results: [");
        for (size_t i = 0; i < count; ++i) {
            if (i > 0) printf(", ");
            printf("%llu", results[i]);
        }
        printf("]\n");
    } else {
        return 1;
    }
    
    return 0;
}
