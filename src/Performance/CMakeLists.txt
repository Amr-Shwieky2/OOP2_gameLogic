# Performance module CMakeLists.txt

# Source files - explicitly list all files
set(PERFORMANCE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Profiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceOverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryTracker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceDemo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ComponentPerformanceMonitor.cpp
    # Add any other source files here
)

# Header files
set(PERFORMANCE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Performance/Profiler.h
    ${CMAKE_SOURCE_DIR}/include/Performance/PerformanceOverlay.h
    ${CMAKE_SOURCE_DIR}/include/Performance/MemoryTracker.h
    ${CMAKE_SOURCE_DIR}/include/Performance/PerformanceDemo.h
    ${CMAKE_SOURCE_DIR}/include/Performance/ComponentPerformanceMonitor.h
    # Add any other header files here
)

# Add source files directly to the library
target_sources(
    ${CMAKE_PROJECT_NAME}_lib
    PRIVATE ${PERFORMANCE_SOURCES} ${PERFORMANCE_HEADERS}
)

# Propagate header list up so the parent can include it in IDE grouping
set(GAME_HEADERS ${GAME_HEADERS} ${PERFORMANCE_HEADERS} PARENT_SCOPE)
