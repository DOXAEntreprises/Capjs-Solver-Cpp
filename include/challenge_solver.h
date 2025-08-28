#pragma once

#include <string>
#include <vector>
#include <functional>
#include <future>
#include <cstdint>

namespace cap {

/**
 * Configuration for challenge solving
 */
struct SolveConfig {
    size_t c = 1;               // Number of challenges to generate
    size_t s = 32;              // Salt length
    size_t d = 6;               // Difficulty (target length)
    size_t worker_count = 0;    // Number of workers (0 = auto)
    std::function<void(int, int, int, uint64_t)> on_progress = nullptr;  // progress callback
};

/**
 * Result of a single challenge
 */
struct ChallengeResult {
    uint64_t nonce;
    size_t challenge_index;
    double duration_seconds;
};

/**
 * Multi-threaded challenge solver
 */
class ChallengeSolver {
public:
    /**
     * Solve single challenge or generate and solve multiple challenges
     * @param challenge Either a single challenge string or array of [salt, target] pairs
     * @param config Configuration options
     * @return Vector of nonces in order
     */
    static std::vector<uint64_t> solve(const std::string& challenge, 
                                     const SolveConfig& config = {});
    
    /**
     * Solve pre-generated challenges
     * @param challenges Vector of [salt, target] pairs
     * @param config Configuration options
     * @return Vector of nonces in order
     */
    static std::vector<uint64_t> solve(const std::vector<std::pair<std::string, std::string>>& challenges,
                                     const SolveConfig& config = {});

private:
    /**
     * Generate challenges from seed
     * @param seed Base seed string
     * @param count Number of challenges
     * @param salt_length Salt length
     * @param difficulty Difficulty (target length)
     * @return Vector of [salt, target] pairs
     */
    static std::vector<std::pair<std::string, std::string>> 
    generate_challenges(const std::string& seed, size_t count, size_t salt_length, size_t difficulty);
    
    /**
     * Solve a single challenge in a worker thread
     * @param salt Salt string
     * @param target Target string
     * @param challenge_index Index of this challenge
     * @return Result with nonce, index, and timing
     */
    static ChallengeResult solve_single(const std::string& salt, 
                                      const std::string& target, 
                                      size_t challenge_index);
};

} // namespace cap
