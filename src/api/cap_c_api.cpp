#include "../../include/cap_c_api.h"
#include "../../include/pow_solver.h"
#include "../../include/prng.h"
#include "../../include/challenge_solver.h"
#include <cstring>

extern "C" {

uint64_t cap_solve_pow(const char* salt, const char* target) {
    if (!salt || !target) {
        return UINT64_MAX;
    }
    
    try {
        return cap::PowSolver::solve_pow(std::string(salt), std::string(target));
    } catch (...) {
        return UINT64_MAX;
    }
}

int cap_prng_generate(const char* seed, size_t length, char* output) {
    if (!seed || !output) {
        return -1;
    }
    
    try {
        const std::string result = cap::PRNG::generate(std::string(seed), length);
        std::strncpy(output, result.c_str(), length);
        output[length] = '\0';
        return 0;
    } catch (...) {
        return -1;
    }
}

int cap_solve_challenges(const char* seed, size_t count, size_t salt_length, 
                        size_t difficulty, uint64_t* results) {
    if (!seed || !results || count == 0) {
        return -1;
    }
    
    try {
        cap::SolveConfig config;
        config.c = count;
        config.s = salt_length;
        config.d = difficulty;
        
        const auto cpp_results = cap::ChallengeSolver::solve(std::string(seed), config);
        
        for (size_t i = 0; i < count && i < cpp_results.size(); ++i) {
            results[i] = cpp_results[i];
        }
        
        return 0;
    } catch (...) {
        return -1;
    }
}

} // extern "C"
