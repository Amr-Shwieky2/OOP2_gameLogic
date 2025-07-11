#pragma once

#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <string>
#include <memory>
#include <chrono>
#include <vector>

// Forward declarations for memory pool demo
class MemoryPoolBase;
template<typename T, size_t BlockSize, size_t InitialCapacity>
class ObjectPool;

/**
 * TemplateMetaProgrammingDemo
 * 
 * This file contains various template meta-programming examples and utilities
 * including compile-time calculations, SFINAE techniques, and memory pool
 * demonstrations with compile-time configuration.
 */

// Compile-time factorial calculation
template<unsigned int N>
struct Factorial {
    static constexpr unsigned int value = N * Factorial<N - 1>::value;
};

// Base case for factorial
template<>
struct Factorial<0> {
    static constexpr unsigned int value = 1;
};

// Compile-time power calculation
template<typename T, T Base, unsigned int Exponent>
struct Power {
    static constexpr T value = Base * Power<T, Base, Exponent - 1>::value;
};

// Base case for power
template<typename T, T Base>
struct Power<T, Base, 0> {
    static constexpr T value = 1;
};

// Compile-time fibonacci calculation
template<unsigned int N>
struct Fibonacci {
    static constexpr unsigned int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

// Base cases for fibonacci
template<> struct Fibonacci<0> { static constexpr unsigned int value = 0; };
template<> struct Fibonacci<1> { static constexpr unsigned int value = 1; };

// Compile-time GCD calculation
template<unsigned int A, unsigned int B>
struct GCD {
    static constexpr unsigned int value = GCD<B, A % B>::value;
};

// Base case for GCD
template<unsigned int A>
struct GCD<A, 0> {
    static constexpr unsigned int value = A;
};

// Type traits demonstration
template<typename T>
struct IsPointerLike {
    static constexpr bool value = std::is_pointer<T>::value || 
                                  std::is_member_pointer<T>::value ||
                                  std::is_null_pointer<T>::value;
};

// SFINAE demonstration
// Only defined for integral types
template<typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type
isEven(T value) {
    return value % 2 == 0;
}

// Memory pool with compile-time size configuration
template<typename T, size_t BlockSize = sizeof(T), size_t InitialCapacity = 32>
class CompileTimeConfiguredPool {
public:
    static constexpr size_t blockSize = BlockSize;
    static constexpr size_t initialCapacity = InitialCapacity;
    
    // Type traits checks
    static constexpr bool isValidBlockSize = BlockSize >= sizeof(T);
    static constexpr bool isValidCapacity = InitialCapacity > 0;
    
    // Will fail at compile time if the configuration is invalid
    static_assert(isValidBlockSize, "Block size must be at least sizeof(T)");
    static_assert(isValidCapacity, "Initial capacity must be greater than 0");
    
    // This would be used in the real implementation
    void* allocate() {
        return nullptr; // Placeholder
    }
    
    void deallocate(void* ptr) {
        // Placeholder
    }
};

// Function to run the meta-programming demo
void runMetaProgrammingDemo();

// Function to run the memory pool performance benchmark
void runMemoryPoolBenchmark();