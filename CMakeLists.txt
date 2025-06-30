cmake_minimum_required (VERSION 3.26)

project (oop2_ex04)
set (MY_AUTHORS "YOUR-NAME-HERE")

include (cmake/CompilerSettings.cmake)

add_executable (${CMAKE_PROJECT_NAME})

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
if (NOT MSVC)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
endif()

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (resources)
add_subdirectory (external/box2d/src)

include (cmake/Zip.cmake)

# Locate SFML. If SFML is installed in a custom location set SFML_DIR accordingly
find_package (SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

target_link_libraries (${CMAKE_PROJECT_NAME}
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        box2d
)

target_sources (${CMAKE_PROJECT_NAME} PRIVATE include/miniaudio.h)

# On Windows copy the required SFML DLLs to the output directory
if (WIN32)
    include (cmake/SFML.cmake)
endif ()


