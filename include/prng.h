#pragma once

#include <string>
#include <cstdint>

namespace cap {

/**
 * Pseudo-random number generator using FNV-1a hash and xorshift
 */
class PRNG {
public:
    /**
     * Generate pseudo-random hex string
     * @param seed The seed string
     * @param length The desired length of output
     * @return Hex string of specified length
     */
    static std::string generate(const std::string& seed, size_t length);

private:
    /**
     * FNV-1a hash function
     * @param str Input string
     * @return 32-bit hash value
     */
    static uint32_t fnv1a(const std::string& str);
    
    /**
     * Xorshift PRNG next value
     * @param state Current state (modified)
     * @return Next random value
     */
    static uint32_t xorshift_next(uint32_t& state);
};

} // namespace cap
