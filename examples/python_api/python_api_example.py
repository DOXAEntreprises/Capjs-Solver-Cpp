import ctypes
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

class CapSolver:
    """Python wrapper for CAP solver"""
    
    def __init__(self, lib_path: str = "libcap_solver.so"):
        self.native = CapNative(lib_path)
    
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
    """Example usage"""
    solver = CapSolver()
    
    # Example 1: Single challenge
    nonce = solver.solve_pow("test", "00")
    if nonce is not None:
        print(f"nonce: {nonce}")
    
    # Example 2: PRNG
    prng = solver.generate_prng("test", 16)
    if prng is not None:
        print(f"prng: {prng}")
    
    # Example 3: Multiple challenges
    results = solver.solve_challenges("test", 3, 8, 1)
    if results is not None:
        print(f"results: {results}")

if __name__ == "__main__":
    main()
