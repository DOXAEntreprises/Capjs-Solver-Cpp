#include "../../include/prng.h"
#include <string>
#include <cstdio>

namespace cap {

std::string PRNG::generate(const std::string& seed, size_t length) {
    uint32_t state = fnv1a(seed);
    std::string result;
    result.reserve(length);
    
    while (result.length() < length) {
        const uint32_t rnd = xorshift_next(state);
        
        // Convert to hex string (8 characters for 32-bit value)
        char hex_str[9];
        snprintf(hex_str, sizeof(hex_str), "%08x", rnd);
        result += hex_str;
    }
    
    return result.substr(0, length);
}

uint32_t PRNG::fnv1a(const std::string& str) {
    uint32_t hash = 2166136261U;
    
    for (char c : str) {
        hash ^= static_cast<uint32_t>(static_cast<uint8_t>(c));
        hash += (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
    }
    
    return hash;
}

uint32_t PRNG::xorshift_next(uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

} // namespace cap
