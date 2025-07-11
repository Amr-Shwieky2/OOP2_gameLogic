# Test fixtures subdirectory
file(GLOB_RECURSE FIXTURES_SOURCES CONFIGURE_DEPENDS LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

# Add the fixture sources to the test executable
target_sources(run_tests PRIVATE ${FIXTURES_SOURCES})

# Group fixtures in Visual Studio IDE
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX "Fixtures" FILES ${FIXTURES_SOURCES})

# Fixtures CMakeLists.txt
file(GLOB FIXTURE_HEADERS
    "*.h"
)

# Add the fixture headers to the test executable
target_sources(run_tests PRIVATE ${FIXTURE_HEADERS})

# Create test_scripts directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_scripts)

# Create source groups for IDE organization
source_group("Fixtures" FILES ${FIXTURE_HEADERS})