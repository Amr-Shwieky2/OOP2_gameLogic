# Integration tests CMakeLists.txt

file(GLOB INTEGRATION_TEST_SOURCES
    "*.cpp"
)

# Add the integration test sources to the test executable
target_sources(run_tests PRIVATE ${INTEGRATION_TEST_SOURCES})

# Create source groups for IDE organization
source_group("Integration Tests" FILES ${INTEGRATION_TEST_SOURCES})