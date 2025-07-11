cmake_minimum_required(VERSION 3.26)

# Testing framework setup
project(OOP2_Tests)

# Find GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0 # Use the latest stable release
)
FetchContent_MakeAvailable(googletest)

# Find Google Benchmark for performance testing
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3 # Use the latest stable release
)
FetchContent_MakeAvailable(benchmark)

# Add compiler flags for test code
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/tests/mocks
  ${CMAKE_SOURCE_DIR}/tests/fixtures
)

# Use the stub Lua implementation if created in the main project
if(TARGET lua_lib)
  get_target_property(LUA_INCLUDE_DIR lua_lib INTERFACE_INCLUDE_DIRECTORIES)
  set(LUA_LIBRARIES lua_lib)
  set(LUA_FOUND TRUE)
endif()

# Collect all test source files
file(GLOB_RECURSE TEST_SOURCES
  "*.cpp"
)

# Create test executable
add_executable(run_tests ${TEST_SOURCES})

# Include Lua headers and link against it
if(LUA_INCLUDE_DIR)
  target_include_directories(run_tests PRIVATE ${LUA_INCLUDE_DIR})
endif()

if(DEFINED LUA_LIBRARIES)
  target_link_libraries(run_tests PRIVATE ${LUA_LIBRARIES})
endif()

# Link test executable with Google Test, Google Benchmark, and other libraries
target_link_libraries(run_tests PRIVATE
  gtest
  gtest_main
  gmock
  benchmark::benchmark
  ${CMAKE_PROJECT_NAME}_lib
  sfml-graphics
  sfml-window
  sfml-system
  box2d
)

# Add test subdirectories
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(performance)
add_subdirectory(mocks)
add_subdirectory(fixtures)

# Create necessary directories for tests
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/test_scripts)

# Create test report generation target
add_custom_target(test_report
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/reports
  COMMAND $<TARGET_FILE:run_tests> --gtest_output=xml:${CMAKE_BINARY_DIR}/reports/test_results.xml
  DEPENDS run_tests
)

# Add code coverage if using GCC
if(CMAKE_COMPILER_IS_GNUCXX)
  include(CodeCoverage.cmake)
  APPEND_COVERAGE_COMPILER_FLAGS()
  
  set(COVERAGE_EXCLUDES 
    '${CMAKE_SOURCE_DIR}/tests/*'
    '${CMAKE_SOURCE_DIR}/external/*'
  )
  
  SETUP_TARGET_FOR_COVERAGE_LCOV(
    NAME coverage
    EXECUTABLE run_tests
    DEPENDENCIES run_tests
  )
endif()