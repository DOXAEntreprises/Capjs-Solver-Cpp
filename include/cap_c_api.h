#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * C API for the CAP C++ Solver
 * This allows the library to be used from C or other languages
 */

/**
 * Solve a single proof of work challenge
 * @param salt Null-terminated salt string
 * @param target Null-terminated hex target string
 * @return The nonce that solves the challenge, or UINT64_MAX on error
 */
uint64_t cap_solve_pow(const char* salt, const char* target);

/**
 * Generate a pseudo-random hex string
 * @param seed Null-terminated seed string
 * @param length Desired output length
 * @param output Buffer to store the result (must be at least length+1 bytes)
 * @return 0 on success, -1 on error
 */
int cap_prng_generate(const char* seed, size_t length, char* output);

/**
 * Solve multiple generated challenges
 * @param seed Null-terminated seed string
 * @param count Number of challenges to generate
 * @param salt_length Length of generated salts
 * @param difficulty Difficulty (target length)
 * @param results Array to store nonces (must have space for 'count' elements)
 * @return 0 on success, -1 on error
 */
int cap_solve_challenges(const char* seed, size_t count, size_t salt_length, 
                        size_t difficulty, uint64_t* results);

#ifdef __cplusplus
}
#endif
