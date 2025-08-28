#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace cap {

/**
 * Simple SHA256 implementation
 */
class SHA256 {
public:
    SHA256();
    void update(const uint8_t* data, size_t length);
    void update(const std::string& data);
    std::vector<uint8_t> finalize();

private:
    void transform();
    void pad();
    
    uint32_t h[8];
    uint8_t buffer[64];
    size_t buffer_length;
    uint64_t total_length;
};

} // namespace cap
