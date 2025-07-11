cmake_minimum_required (VERSION 3.26)

project (oop2_ex04)
set (MY_AUTHORS "YOUR-NAME-HERE")

# Add our custom cmake modules directory to the search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (cmake/CompilerSettings.cmake)

# Create a shared library that we can link to from tests
add_library(${CMAKE_PROJECT_NAME}_lib SHARED)

# Create the main executable
add_executable (${CMAKE_PROJECT_NAME})

# Set compiler options
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
target_compile_options(${CMAKE_PROJECT_NAME}_lib PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)

if (NOT MSVC)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
    target_link_options(${CMAKE_PROJECT_NAME}_lib PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
endif()

# Set SFML paths
set (SFML_LOCATION "C:/SFML/SFML-2.6.1")
set (SFML_DIR "${SFML_LOCATION}/lib/cmake/SFML")

# Find SFML
find_package (SFML 2.6 COMPONENTS graphics REQUIRED)

# Set up SFML target properties
set_target_properties (sfml-main sfml-system sfml-window sfml-graphics sfml-audio sfml-network 
                       PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

# Set include directories
target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# First add src directory which will build Lua if needed
add_subdirectory (src)
add_subdirectory (include)
add_subdirectory (resources)
add_subdirectory (external/box2d/src)

# Link the main executable with the library
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_PROJECT_NAME}_lib)

# Link dependencies to the shared library
target_link_libraries (${CMAKE_PROJECT_NAME}_lib PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio         
    sfml-main
    box2d
)

# Disable tests temporarily
#option(BUILD_TESTS "Build the test suite" OFF)
#if(BUILD_TESTS)
#    enable_testing()
#    add_subdirectory(tests)
#endif()

include (cmake/Zip.cmake)

# Create directories for scripts
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/scripts
)

# Create directories for test scripts
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/test_scripts
)

include (cmake/SFML.cmake)

