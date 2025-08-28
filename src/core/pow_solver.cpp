#include "../../include/pow_solver.h"
#include "../../include/sha256.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

namespace cap {

uint64_t PowSolver::solve_pow(const std::string& salt, const std::string& target) {
    const auto target_bytes = parse_hex_target(target);
    const size_t target_bits = target.length() * 4; // each hex char = 4 bits
    
    for (uint64_t nonce = 0; nonce != std::numeric_limits<uint64_t>::max(); ++nonce) {
        const std::string nonce_str = uint64_to_string(nonce);
        
        // Calculate SHA256 hash
        SHA256 hasher;
        hasher.update(salt);
        hasher.update(nonce_str);
        const auto hash_vec = hasher.finalize();
        
        if (hash_matches_target(hash_vec, target_bytes, target_bits)) {
            return nonce;
        }
    }
    
    // Should never reach here
    throw std::runtime_error("Solution should be found before exhausting uint64_t range");
}

std::vector<uint8_t> PowSolver::parse_hex_target(const std::string& target) {
    std::string padded_target = target;
    
    // Pad with '0' if odd length
    if (padded_target.length() % 2 != 0) {
        padded_target.push_back('0');
    }
    
    std::vector<uint8_t> result;
    result.reserve(padded_target.length() / 2);
    
    for (size_t i = 0; i < padded_target.length(); i += 2) {
        const std::string byte_str = padded_target.substr(i, 2);
        const uint8_t byte_val = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
        result.push_back(byte_val);
    }
    
    return result;
}

std::string PowSolver::uint64_to_string(uint64_t value) {
    if (value == 0) {
        return "0";
    }
    
    std::string result;
    while (value > 0) {
        result = char('0' + (value % 10)) + result;
        value /= 10;
    }
    
    return result;
}

bool PowSolver::hash_matches_target(const std::vector<uint8_t>& hash, 
                                  const std::vector<uint8_t>& target_bytes, 
                                  size_t target_bits) {
    const size_t full_bytes = target_bits / 8;
    const size_t remaining_bits = target_bits % 8;
    
    // Compare full bytes
    for (size_t i = 0; i < full_bytes && i < target_bytes.size(); ++i) {
        if (hash[i] != target_bytes[i]) {
            return false;
        }
    }
    
    // Compare remaining bits if any
    if (remaining_bits > 0 && full_bytes < target_bytes.size() && full_bytes < hash.size()) {
        const uint8_t mask = 0xFF << (8 - remaining_bits);
        const uint8_t hash_masked = hash[full_bytes] & mask;
        const uint8_t target_masked = target_bytes[full_bytes] & mask;
        return hash_masked == target_masked;
    }
    
    return true;
}

} // namespace cap
