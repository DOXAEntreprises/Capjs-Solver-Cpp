#!/usr/bin/env python3
"""
CAP Solver Command Line Tool
Simple CLI interface for the CAP C++ Solver
"""

import ctypes
import argparse
import sys
from typing import List, Optional

class CapNative:
    """ctypes wrapper for CAP C API"""
    
    def __init__(self, lib_path: str = "libcap_solver.so"):
        # Try to load the library
        try:
            self.lib = ctypes.CDLL(lib_path)
        except OSError:
            # Try common locations
            for path in ["./libcap_solver.so", "/usr/lib/libcap_solver.so", 
                        "./cap_solver.dll", "./libcap_solver.dylib"]:
                try:
                    self.lib = ctypes.CDLL(path)
                    break
                except OSError:
                    continue
            else:
                raise RuntimeError("CAP solver library not found")
        
        # Define function signatures
        self.lib.cap_solve_pow.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self.lib.cap_solve_pow.restype = ctypes.c_uint64
        
        self.lib.cap_prng_generate.argtypes = [ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p]
        self.lib.cap_prng_generate.restype = ctypes.c_int
        
        self.lib.cap_solve_challenges.argtypes = [ctypes.c_char_p, ctypes.c_size_t, 
                                                 ctypes.c_size_t, ctypes.c_size_t, 
                                                 ctypes.POINTER(ctypes.c_uint64)]
        self.lib.cap_solve_challenges.restype = ctypes.c_int

class CapCLI:
    """Command line interface for CAP solver"""
    
    def __init__(self):
        self.native = CapNative()
    
    def solve_pow(self, salt: str, target: str) -> Optional[int]:
        """Solve single proof of work challenge"""
        result = self.native.lib.cap_solve_pow(
            salt.encode('utf-8'), 
            target.encode('utf-8')
        )
        return result if result != ctypes.c_uint64(-1).value else None
    
    def generate_prng(self, seed: str, length: int) -> Optional[str]:
        """Generate pseudo-random hex string"""
        buffer = ctypes.create_string_buffer(length + 1)
        result = self.native.lib.cap_prng_generate(
            seed.encode('utf-8'), 
            length, 
            buffer
        )
        return buffer.value.decode('utf-8') if result == 0 else None
    
    def solve_challenges(self, seed: str, count: int, salt_length: int, difficulty: int) -> Optional[List[int]]:
        """Solve multiple generated challenges"""
        results = (ctypes.c_uint64 * count)()
        result = self.native.lib.cap_solve_challenges(
            seed.encode('utf-8'),
            count,
            salt_length,
            difficulty,
            results
        )
        return list(results) if result == 0 else None

def main():
    """Main CLI function"""
    parser = argparse.ArgumentParser(description='CAP Solver CLI Tool')
    subparsers = parser.add_subparsers(dest='command', help='Commands')
    
    # Single challenge command
    pow_parser = subparsers.add_parser('pow', help='Solve single proof of work challenge')
    pow_parser.add_argument('salt', help='Salt string')
    pow_parser.add_argument('target', help='Target hex string')
    
    # PRNG command
    prng_parser = subparsers.add_parser('prng', help='Generate pseudo-random string')
    prng_parser.add_argument('seed', help='Seed string')
    prng_parser.add_argument('length', type=int, help='Output length')
    
    # Multiple challenges command
    challenges_parser = subparsers.add_parser('challenges', help='Solve multiple challenges')
    challenges_parser.add_argument('seed', help='Seed string')
    challenges_parser.add_argument('count', type=int, help='Number of challenges')
    challenges_parser.add_argument('salt_length', type=int, help='Salt length')
    challenges_parser.add_argument('difficulty', type=int, help='Difficulty level')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        cli = CapCLI()
        
        if args.command == 'pow':
            nonce = cli.solve_pow(args.salt, args.target)
            if nonce is not None:
                print(f"nonce: {nonce}")
            else:
                print("error: failed to solve challenge")
                return 1
        
        elif args.command == 'prng':
            prng = cli.generate_prng(args.seed, args.length)
            if prng is not None:
                print(f"prng: {prng}")
            else:
                print("error: failed to generate PRNG")
                return 1
        
        elif args.command == 'challenges':
            results = cli.solve_challenges(args.seed, args.count, args.salt_length, args.difficulty)
            if results is not None:
                print(f"results: {results}")
            else:
                print("error: failed to solve challenges")
                return 1
        
        return 0
    
    except Exception as e:
        print(f"error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
