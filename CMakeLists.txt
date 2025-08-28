cmake_minimum_required(VERSION 3.15)
project(cpp_solver VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" ON)

include_directories(include)

add_library(cap_solver STATIC
    src/core/pow_solver.cpp
    src/core/prng.cpp
    src/core/challenge_solver.cpp
    src/core/sha256.cpp
    src/api/cap_c_api.cpp
)

# Also build shared library for Python/C# bindings
if(BUILD_SHARED_LIBS)
    add_library(cap_solver_shared SHARED
        src/core/pow_solver.cpp
        src/core/prng.cpp
        src/core/challenge_solver.cpp
        src/core/sha256.cpp
        src/api/cap_c_api.cpp
    )
    
    # Set shared library output name
    set_target_properties(cap_solver_shared PROPERTIES OUTPUT_NAME cap_solver)
    
    target_link_libraries(cap_solver_shared 
        Threads::Threads
    )
    
    # Alias for easier linking
    add_library(cap::solver_shared ALIAS cap_solver_shared)
endif()

target_link_libraries(cap_solver 
    Threads::Threads
)

# Alias for easier linking
add_library(cap::solver ALIAS cap_solver)

add_executable(solver
    src/apps/main.cpp
)

target_link_libraries(solver cap_solver)

# Examples
if(BUILD_EXAMPLES)
    add_executable(c_api_example
        examples/c_api/c_api_example.c
    )
    target_link_libraries(c_api_example cap_solver)

    add_executable(cpp_api_example
        examples/cpp_api/cpp_api_example.cpp
    )
    target_link_libraries(cpp_api_example cap_solver)
endif()

# Tests
if(BUILD_TESTS)
    add_executable(local_test
        tests/local.cpp
    )
    target_link_libraries(local_test cap_solver)

    add_executable(standalone_test
        tests/standalone.cpp
    )
    target_link_libraries(standalone_test cap_solver)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(cap_solver PRIVATE -Wall -Wextra -O3)
    if(BUILD_SHARED_LIBS)
        target_compile_options(cap_solver_shared PRIVATE -Wall -Wextra -O3)
    endif()
    target_compile_options(solver PRIVATE -Wall -Wextra -O3)
    
    if(BUILD_EXAMPLES)
        target_compile_options(c_api_example PRIVATE -Wall -Wextra -O3)
        target_compile_options(cpp_api_example PRIVATE -Wall -Wextra -O3)
    endif()
    
    if(BUILD_TESTS)
        target_compile_options(local_test PRIVATE -Wall -Wextra -O3)
        target_compile_options(standalone_test PRIVATE -Wall -Wextra -O3)
    endif()
endif()

# Install targets for shared library
if(BUILD_SHARED_LIBS)
    install(TARGETS cap_solver_shared
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
    )
endif()

# Install static library
install(TARGETS cap_solver
    ARCHIVE DESTINATION lib
)

# Install headers
install(FILES 
    include/cap_c_api.h
    include/challenge_solver.h
    include/pow_solver.h
    include/prng.h
    DESTINATION include/cap
)

# Custom targets for convenience
add_custom_target(examples)
add_custom_target(tests)

if(BUILD_EXAMPLES)
    add_dependencies(examples c_api_example cpp_api_example)
endif()

if(BUILD_TESTS)
    add_dependencies(tests local_test standalone_test)
endif()

# Print build configuration
message(STATUS "Cap.js Solver build configuration:")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
