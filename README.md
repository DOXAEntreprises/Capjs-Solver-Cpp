# Cap.js C++ Solver

A high-performance C++ implementation of the Cap.js solver.

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- CMake 3.15+
- pthread support

## Build

### Quick Start

```bash
git clone https://github.com/LxHTT/Capjs-Solver-Cpp.git

cd build

cmake ..

make -j$(nproc)
```

### Build Options

```bash
# Default build (shared libs, examples, tests)
cmake ..

# Static library only
cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF ..

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Build Targets

```bash
make                   # Build all enabled targets
make solver            # Main solver executable
make examples          # Build all examples
make tests             # Build all tests
```

## Usage

### C++ API

```cpp
#include "challenge_solver.h"

cap::SolveConfig config;
config.c = 5;        // 5 challenges
config.s = 16;       // 16-byte salt
config.d = 3;        // difficulty 3

auto solutions = cap::ChallengeSolver::solve("token", config);
```

### C API

```c
#include "cap_c_api.h"

uint64_t nonce = cap_solve_pow("salt", "00");
uint64_t results[3];
cap_solve_challenges("token", 3, 16, 3, results);
```

### Python API

```python
from examples.python_api.python_api_example import CapSolver
solver = CapSolver()
nonce = solver.solve_pow("salt", "00")
```

### C# API

```bash
# Build C# examples
cd examples/csharp_api
dotnet build CsharpApiExample.csproj
dotnet run
```

## Library Usage

After installation, link against the library:

```bash
# Static library
g++ -lcap_solver -pthread my_app.cpp

# Shared library  
g++ -lcap_solver my_app.cpp
```

## Output Format

All tools use minimal output format:

- `nonce: 12345` - Single challenge result
- `solutions: [123, 456, 789]` - Multiple results
- `error: message` - Error messages

## License

See LICENSE file in repository root.
