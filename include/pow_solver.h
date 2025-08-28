#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace cap {

/**
 * Core Proof of Work solver
 */
class PowSolver {
public:
    /**
     * Solve proof of work challenge
     * @param salt The salt string
     * @param target The hex target string (prefix that hash must match)
     * @return The nonce that satisfies the challenge
     */
    static uint64_t solve_pow(const std::string& salt, const std::string& target);

private:
    /**
     * Parse hex target string to bytes
     * @param target Hex string target
     * @return Vector of bytes
     */
    static std::vector<uint8_t> parse_hex_target(const std::string& target);
    
    /**
     * Convert uint64 to string representation
     * @param value The value to convert
     * @return String representation
     */
    static std::string uint64_to_string(uint64_t value);
    
    /**
     * Check if hash matches target
     * @param hash The hash bytes
     * @param target_bytes The target bytes
     * @param target_bits Number of target bits
     * @return True if hash matches target
     */
    static bool hash_matches_target(const std::vector<uint8_t>& hash, 
                                  const std::vector<uint8_t>& target_bytes, 
                                  size_t target_bits);
};

} // namespace cap
