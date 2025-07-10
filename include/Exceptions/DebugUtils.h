// DebugUtils.h - Utilities for debugging and diagnostics
#pragma once

#include "Logger.h"
#include "Exception.h"
#include <string>
#include <sstream>
#include <typeinfo>
#include <memory>
#include <source_location>
#include <format>

namespace GameExceptions {

/**
 * @brief Get a string representation of a type
 * 
 * This function returns a human-readable name for the given type,
 * with special handling for common types.
 */
template<typename T>
std::string getTypeName() {
    try {
        // Try to get a readable name
        return typeid(T).name();
    }
    catch (...) {
        return "unknown_type";
    }
}

/**
 * @brief Assert a condition, throwing an exception if it fails
 * 
 * This macro-like function checks if a condition is true, and if not,
 * throws an exception with detailed information.
 */
inline void assertThat(bool condition, 
                     const std::string& message = "Assertion failed",
                     Exception::Severity severity = Exception::Severity::Error,
                     const std::source_location& location = std::source_location::current()) {
    if (!condition) {
        throw Exception(message, severity, 0, location);
    }
}

/**
 * @brief Check if a pointer is not null, throwing an exception if it is
 * 
 * This is useful for validating pointers before use.
 */
template<typename T>
T* notNull(T* pointer, 
         const std::string& pointerName = "pointer",
         const std::source_location& location = std::source_location::current()) {
    if (pointer == nullptr) {
        throw Exception(std::format("Null pointer: {}", pointerName), 
                      Exception::Severity::Error, 
                      0, 
                      location);
    }
    return pointer;
}

/**
 * @brief Check if a value is within a range, throwing an exception if not
 * 
 * Useful for validating numeric parameters.
 */
template<typename T>
T inRange(T value, 
        T minValue, 
        T maxValue,
        const std::string& valueName = "value",
        const std::source_location& location = std::source_location::current()) {
    if (value < minValue || value > maxValue) {
        throw Exception(
            std::format("Value {} out of range: {} (should be between {} and {})",
                      valueName, value, minValue, maxValue),
            Exception::Severity::Error,
            0,
            location);
    }
    return value;
}

/**
 * @brief Measure execution time of a function
 * 
 * This helper function runs a given function and logs how long it takes.
 */
template<typename Func>
auto measureExecutionTime(Func&& func, 
                        const std::string& operationName = "Operation",
                        const std::source_location& location = std::source_location::current()) {
    auto start = std::chrono::steady_clock::now();
    
    // Call the function and get its result
    if constexpr (std::is_void_v<std::invoke_result_t<Func>>) {
        // Function returns void
        func();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        getLogger().debug(std::format("{} completed in {} ms", operationName, duration), location);
    } 
    else {
        // Function returns a value
        auto result = func();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        getLogger().debug(std::format("{} completed in {} ms", operationName, duration), location);
        return result;
    }
}

} // namespace GameExceptions

// Convenience macros for assertions and null checks
#define ASSERT_THAT(condition, message) \
    GameExceptions::assertThat(condition, message)

#define NOT_NULL(pointer, name) \
    GameExceptions::notNull(pointer, name)

#define IN_RANGE(value, min, max, name) \
    GameExceptions::inRange(value, min, max, name)