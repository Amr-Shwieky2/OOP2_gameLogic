# Performance tests CMakeLists.txt

file(GLOB PERFORMANCE_TEST_SOURCES
    "*.cpp"
)

# Add the performance test sources to the test executable
target_sources(run_tests PRIVATE ${PERFORMANCE_TEST_SOURCES})

# Create source groups for IDE organization
source_group("Performance Tests" FILES ${PERFORMANCE_TEST_SOURCES})