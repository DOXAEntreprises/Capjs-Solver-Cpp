#include "../../include/challenge_solver.h"
#include "../../include/pow_solver.h"
#include "../../include/prng.h"
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>

namespace cap {

std::vector<uint64_t> ChallengeSolver::solve(const std::string& challenge, 
                                           const SolveConfig& config) {
    // Generate challenges from seed
    auto challenges = generate_challenges(challenge, config.c, config.s, config.d);
    return solve(challenges, config);
}

std::vector<uint64_t> ChallengeSolver::solve(const std::vector<std::pair<std::string, std::string>>& challenges,
                                           const SolveConfig& config) {
    const size_t total_challenges = challenges.size();
    
    if (total_challenges == 0) {
        return {};
    }
    
    // Determine number of workers
    size_t worker_count = config.worker_count;
    if (worker_count == 0) {
        worker_count = std::min(total_challenges, static_cast<size_t>(std::thread::hardware_concurrency()));
    }
    
    std::vector<uint64_t> results(total_challenges);
    std::vector<std::future<ChallengeResult>> futures;
    futures.reserve(total_challenges);
    
    // Launch all challenges asynchronously
    for (size_t i = 0; i < total_challenges; ++i) {
        const std::string& salt = challenges[i].first;
        const std::string& target = challenges[i].second;
        
        futures.emplace_back(std::async(std::launch::async, [salt, target, i]() {
            return solve_single(salt, target, i);
        }));
    }
    
    // Collect results as they complete
    size_t completed = 0;
    for (auto& future : futures) {
        const auto result = future.get();
        results[result.challenge_index] = result.nonce;
        
        completed++;
        
        // Call progress callback if provided
        if (config.on_progress) {
            const int progress = static_cast<int>((completed * 100) / total_challenges);
            config.on_progress(progress, static_cast<int>(result.challenge_index), 
                             static_cast<int>(completed), result.nonce);
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, std::string>> 
ChallengeSolver::generate_challenges(const std::string& seed, size_t count, 
                                   size_t salt_length, size_t difficulty) {
    std::vector<std::pair<std::string, std::string>> challenges;
    challenges.reserve(count);
    
    for (size_t i = 1; i <= count; ++i) {
        const std::string salt_seed = seed + std::to_string(i);
        const std::string target_seed = seed + std::to_string(i) + "d";
        
        const std::string salt = PRNG::generate(salt_seed, salt_length);
        const std::string target = PRNG::generate(target_seed, difficulty);
        
        challenges.emplace_back(salt, target);
    }
    
    return challenges;
}

ChallengeResult ChallengeSolver::solve_single(const std::string& salt, 
                                            const std::string& target, 
                                            size_t challenge_index) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    const uint64_t nonce = PowSolver::solve_pow(salt, target);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration<double>(end_time - start_time);
    
    return {nonce, challenge_index, duration.count()};
}

} // namespace cap
