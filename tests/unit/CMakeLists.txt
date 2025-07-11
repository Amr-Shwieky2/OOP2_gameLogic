# Unit tests CMakeLists.txt

file(GLOB UNIT_TEST_SOURCES
    "*.cpp"
)

# Add the unit test sources to the test executable
target_sources(run_tests PRIVATE ${UNIT_TEST_SOURCES})

# Create source groups for IDE organization
source_group("Unit Tests" FILES ${UNIT_TEST_SOURCES})