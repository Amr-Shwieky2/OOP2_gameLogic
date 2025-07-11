# Comprehensive Testing Framework

This directory contains a comprehensive testing framework for the game engine, providing unit tests, integration tests, performance benchmarks, mock objects, and code coverage reporting.

## Overview

The testing framework includes:

- **Unit Tests**: Tests for individual components and classes
- **Integration Tests**: Tests for interactions between different systems
- **Performance Tests**: Benchmarks and performance regression tests
- **Mock Objects**: Mock implementations of key interfaces for testing
- **Property-Based Testing**: Tests with randomized inputs
- **Fuzz Testing**: Tests with malformed inputs for error handling
- **Test Reports**: Generation of HTML, XML, and CSV test reports

## Directory Structure

- `tests/`
  - `unit/`: Unit tests for individual components
  - `integration/`: Tests for system interactions
  - `performance/`: Performance benchmarks and tests
  - `mocks/`: Mock objects for testing
  - `fixtures/`: Test fixtures and common setup code
  - `reports/`: Generated test reports (created when tests run)

## Requirements

- CMake 3.26 or higher
- C++20 compatible compiler
- Google Test and Google Mock (downloaded automatically by CMake)
- Google Benchmark (downloaded automatically by CMake)
- LCOV (optional, for code coverage reporting)

## Building and Running Tests

### Building Tests

```bash
mkdir build && cd build
cmake ..
cmake --build . --target run_tests
```

### Running Tests

```bash
./build/run_tests
```

### Command Line Options

The test runner supports various command line options:

- `--gtest_filter=<pattern>`: Run only tests that match the pattern
- `--gtest_break_on_failure`: Stop on first failure
- `--gtest_repeat=<count>`: Repeat tests multiple times
- `--no-reports`: Disable report generation
- `--report-dir=<dir>`: Specify report output directory

Example:
```bash
./build/run_tests --gtest_filter=MemoryPoolTest.*
```

## Generating Reports

### Basic Test Reports

Test reports are automatically generated when running tests. Reports are saved in HTML, XML, and CSV formats in the `test_reports` directory.

To explicitly generate reports:

```bash
cmake --build . --target test_report
```

### Code Coverage Reports

To generate code coverage reports (requires GCC and LCOV):

```bash
cmake --build . --target coverage
```

The coverage report will be available in the `build/coverage/` directory.

## Adding New Tests

### Unit Tests

Add new unit test files to the `unit/` directory. Use the Google Test framework:

```cpp
#include <gtest/gtest.h>
#include "ComponentUnderTest.h"

TEST(TestSuiteName, TestName) {
    // Test code
    EXPECT_TRUE(condition);
}
```

### Integration Tests

Add new integration test files to the `integration/` directory:

```cpp
#include <gtest/gtest.h>
#include "SystemA.h"
#include "SystemB.h"

TEST(IntegrationTest, SystemAInteractsWithSystemB) {
    // Test integration between systems
}
```

### Performance Tests

Add new performance test files to the `performance/` directory:

```cpp
#include <benchmark/benchmark.h>

static void BM_SomeFunction(benchmark::State& state) {
    for (auto _ : state) {
        // Code to benchmark
    }
}

BENCHMARK(BM_SomeFunction);
```

### Mock Objects

Add new mock objects to the `mocks/` directory:

```cpp
#include <gmock/gmock.h>
#include "IInterface.h"

class MockInterface : public IInterface {
public:
    MOCK_METHOD(ReturnType, methodName, (ArgType), (override));
};
```

### Test Fixtures

Add new test fixtures to the `fixtures/` directory:

```cpp
#include <gtest/gtest.h>

class MyFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize resources
    }
    
    void TearDown() override {
        // Clean up resources
    }
    
    // Common test objects
};
```

## Best Practices

1. **Test One Thing at a Time**: Each test should verify a single behavior or feature.
2. **Use Descriptive Test Names**: Test names should describe what is being tested and expected behavior.
3. **Use Test Fixtures**: For common setup/teardown code.
4. **Mock External Dependencies**: Use mock objects for external dependencies.
5. **Test Edge Cases**: Include tests for boundary conditions and error cases.
6. **Check for Memory Leaks**: Use the `MemoryFixture` to detect memory leaks in tests.
7. **Keep Tests Fast**: Slow tests discourage running the test suite frequently.
8. **Keep Tests Independent**: Tests should not depend on each other.

## Using Fuzz Testing

The framework includes a `FuzzTestFramework` class for generating random inputs:

```cpp
#include "FuzzTests.h"

// Generate random string
std::string randomStr = FuzzTestFramework::randomString(10);

// Generate random number
int randomNum = FuzzTestFramework::randomNumber(1, 100);

// Generate random position
sf::Vector2f pos = FuzzTestFramework::randomPosition();
```