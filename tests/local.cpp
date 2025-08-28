#include "../include/pow_solver.h"
#include "../include/prng.h"
#include "../include/challenge_solver.h"
#include <iostream>
#include <cassert>

void test_prng() {
    const std::string result1 = cap::PRNG::generate("test", 8);
    const std::string result2 = cap::PRNG::generate("test", 8);
    assert(result1 == result2);
    assert(result1.length() == 8);
    const std::string result3 = cap::PRNG::generate("different", 8);
    assert(result1 != result3);
}

void test_pow_solver() {
    try {
        const uint64_t nonce = cap::PowSolver::solve_pow("test", "0");
        assert(nonce >= 0);
        std::cout << "nonce: " << nonce << std::endl;
    } catch (const std::exception& e) {
        assert(false);
    }
}

void test_challenge_solver() {
    cap::SolveConfig config;
    config.c = 2;
    config.s = 8;
    config.d = 1;
    
    try {
        const auto results = cap::ChallengeSolver::solve("test", config);
        assert(results.size() == 2);
        for (const auto& nonce : results) {
            assert(nonce >= 0);
        }
        std::cout << "results: [";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << results[i];
        }
        std::cout << "]" << std::endl;
    } catch (const std::exception& e) {
        assert(false);
    }
    
    std::vector<std::pair<std::string, std::string>> challenges = {
        {"salt1", "0"},
        {"salt2", "1"}
    };
    
    try {
        const auto results = cap::ChallengeSolver::solve(challenges, config);
        assert(results.size() == 2);
    } catch (const std::exception& e) {
        assert(false);
    }
}

void benchmark() {
    cap::SolveConfig config;
    config.c = 5;
    config.s = 16;
    config.d = 2;
    
    config.on_progress = [](int progress, int current, int completed, uint64_t nonce) {
        std::cout << "challenge " << current << " nonce: " << nonce << std::endl;
    };
    
    try {
        const auto results = cap::ChallengeSolver::solve("benchmark", config);
        std::cout << "results: [";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << results[i];
        }
        std::cout << "]" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        test_prng();
        test_pow_solver();
        test_challenge_solver();
        benchmark();
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
    return 0;
}
