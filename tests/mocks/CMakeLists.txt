# Mock objects subdirectory
file(GLOB_RECURSE MOCKS_SOURCES CONFIGURE_DEPENDS LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

# Add the mock sources to the test executable
target_sources(run_tests PRIVATE ${MOCKS_SOURCES})

# Group mocks in Visual Studio IDE
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX "Mocks" FILES ${MOCKS_SOURCES})

# Mocks CMakeLists.txt
file(GLOB MOCK_HEADERS
    "*.h"
)

# Add the mock headers to the test executable
target_sources(run_tests PRIVATE ${MOCK_HEADERS})

# Create source groups for IDE organization
source_group("Mocks" FILES ${MOCK_HEADERS})