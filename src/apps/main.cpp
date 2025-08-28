#include "../../include/challenge_solver.h"
#include "../../include/pow_solver.h"
#include <iostream>
#include <chrono>

void print_usage() {
    std::cout << "Usage: solver [command] [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  single <salt> <target>           - Solve single challenge\n";
    std::cout << "  generate <seed> [options]        - Generate and solve multiple challenges\n";
    std::cout << "  batch <salt1> <target1> ...      - Solve multiple predefined challenges\n\n";
    std::cout << "Options for generate:\n";
    std::cout << "  -c <count>     Number of challenges (default: 1)\n";
    std::cout << "  -s <length>    Salt length (default: 32)\n";
    std::cout << "  -d <length>    Difficulty/target length (default: 6)\n";
    std::cout << "  -w <workers>   Number of worker threads (default: auto)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  solver single \"mysalt\" \"000abc\"\n";
    std::cout << "  solver generate \"test\" -c 5 -d 4\n";
    std::cout << "  solver batch \"salt1\" \"000a\" \"salt2\" \"000b\"\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "single") {
        if (argc != 4) {
            std::cerr << "Error: single command requires salt and target\n";
            print_usage();
            return 1;
        }
        
        const std::string salt = argv[2];
        const std::string target = argv[3];
        
        std::cout << "Solving single challenge...\n";
        std::cout << "Salt: " << salt << "\n";
        std::cout << "Target: " << target << "\n";
        
        const auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            const uint64_t nonce = cap::PowSolver::solve_pow(salt, target);
            
            const auto end_time = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration<double>(end_time - start_time);
            
            std::cout << "Solution found!\n";
            std::cout << "Nonce: " << nonce << "\n";
            std::cout << "Time: " << duration.count() << " seconds\n";
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        
    } else if (command == "generate") {
        if (argc < 3) {
            std::cerr << "Error: generate command requires seed\n";
            print_usage();
            return 1;
        }
        
        const std::string seed = argv[2];
        cap::SolveConfig config;
        
        // Parse options
        for (int i = 3; i < argc; i += 2) {
            if (i + 1 >= argc) {
                std::cerr << "Error: Option " << argv[i] << " requires a value\n";
                return 1;
            }
            
            std::string option = argv[i];
            std::string value = argv[i + 1];
            
            if (option == "-c") {
                config.c = std::stoull(value);
            } else if (option == "-s") {
                config.s = std::stoull(value);
            } else if (option == "-d") {
                config.d = std::stoull(value);
            } else if (option == "-w") {
                config.worker_count = std::stoull(value);
            } else {
                std::cerr << "Error: Unknown option " << option << "\n";
                return 1;
            }
        }
        
        // Set up progress callback
        config.on_progress = [](int progress, int current, int completed, uint64_t nonce) {
            std::cout << "Progress: " << progress << "% (" << completed << " completed) - "
                      << "Challenge " << current << " nonce: " << nonce << "\n";
        };
        
        std::cout << "Generating and solving " << config.c << " challenges...\n";
        std::cout << "Seed: " << seed << "\n";
        std::cout << "Salt length: " << config.s << "\n";
        std::cout << "Difficulty: " << config.d << "\n";
        std::cout << "Workers: " << (config.worker_count == 0 ? "auto" : std::to_string(config.worker_count)) << "\n\n";
        
        const auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            const auto results = cap::ChallengeSolver::solve(seed, config);
            
            const auto end_time = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration<double>(end_time - start_time);
            
            std::cout << "\nAll challenges solved!\n";
            std::cout << "Total time: " << duration.count() << " seconds\n";
            std::cout << "Results: [";
            for (size_t i = 0; i < results.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << results[i];
            }
            std::cout << "]\n";
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        
    } else if (command == "batch") {
        if (argc < 4 || (argc - 2) % 2 != 0) {
            std::cerr << "Error: batch command requires pairs of salt and target\n";
            print_usage();
            return 1;
        }
        
        std::vector<std::pair<std::string, std::string>> challenges;
        
        for (int i = 2; i < argc; i += 2) {
            challenges.emplace_back(argv[i], argv[i + 1]);
        }
        
        cap::SolveConfig config;
        config.on_progress = [](int progress, int current, int completed, uint64_t nonce) {
            std::cout << "Progress: " << progress << "% (" << completed << " completed) - "
                      << "Challenge " << current << " nonce: " << nonce << "\n";
        };
        
        std::cout << "Solving " << challenges.size() << " predefined challenges...\n\n";
        
        const auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            const auto results = cap::ChallengeSolver::solve(challenges, config);
            
            const auto end_time = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration<double>(end_time - start_time);
            
            std::cout << "\nAll challenges solved!\n";
            std::cout << "Total time: " << duration.count() << " seconds\n";
            std::cout << "Results: [";
            for (size_t i = 0; i < results.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << results[i];
            }
            std::cout << "]\n";
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        
    } else {
        std::cerr << "Error: Unknown command '" << command << "'\n";
        print_usage();
        return 1;
    }
    
    return 0;
}
