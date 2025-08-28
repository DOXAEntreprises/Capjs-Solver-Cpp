#include "../../include/challenge_solver.h"
#include "../../include/pow_solver.h"
#include <iostream>
#include <vector>

int main() {
    // Example 1: Single challenge
    try {
        uint64_t nonce = cap::PowSolver::solve_pow("example_salt", "000");
        std::cout << "nonce: " << nonce << std::endl;
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << std::endl;
    }

    // Example 2: Multiple challenges
    cap::SolveConfig config;
    config.c = 5;
    config.s = 16;
    config.d = 3;
    config.on_progress = [](int progress, int current, int completed, uint64_t nonce) {
        std::cout << "challenge " << current << " nonce: " << nonce << std::endl;
    };

    try {
        std::vector<uint64_t> solutions = cap::ChallengeSolver::solve("multi_challenge_token", config);
        std::cout << "solutions: [";
        for (size_t i = 0; i < solutions.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << solutions[i];
        }
        std::cout << "]" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << std::endl;
    }

    // Example 3: High difficulty challenge
    cap::SolveConfig hard_config;
    hard_config.c = 3;
    hard_config.s = 32;
    hard_config.d = 4;
    hard_config.on_progress = [](int progress, int current, int completed, uint64_t nonce) {
        std::cout << "challenge " << current << " nonce: " << nonce << std::endl;
    };

    try {
        std::vector<uint64_t> hard_solutions = cap::ChallengeSolver::solve("hard_challenge_token", hard_config);
        std::cout << "hard_solutions: [";
        for (size_t i = 0; i < hard_solutions.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << hard_solutions[i];
        }
        std::cout << "]" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << std::endl;
    }

    return 0;
}
