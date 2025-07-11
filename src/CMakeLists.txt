file(GLOB_RECURSE MY_SOURCE_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} 
    *.cpp
    !StateMachine/*.cpp  # Exclude StateMachine source files as they will be added separately
    !Performance/*.cpp   # Exclude Performance source files as they will be added separately
    !Scripting/*.cpp     # Exclude Scripting source files as they will be added separately
)

# First add Scripting subdirectory to ensure Lua is built before other components need it
add_subdirectory(Scripting)

# Add State Machine subdirectory
add_subdirectory(StateMachine)

# Add Performance monitoring subdirectory
add_subdirectory(Performance)

# Add all source files to the library
target_sources(${CMAKE_PROJECT_NAME}_lib PRIVATE ${MY_SOURCE_FILES})

# Add just the main.cpp file to the executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Add a compile definition to enable the validated state machine
target_compile_definitions(${CMAKE_PROJECT_NAME}_lib PRIVATE USE_VALIDATED_STATE_MACHINE)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE USE_VALIDATED_STATE_MACHINE)

# Add compile definitions for performance monitoring
target_compile_definitions(${CMAKE_PROJECT_NAME}_lib PRIVATE 
    ENABLE_PERFORMANCE_MONITORING
    PERFORMANCE_MONITORING_VERSION=1
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE 
    ENABLE_PERFORMANCE_MONITORING
    PERFORMANCE_MONITORING_VERSION=1
)

# Add compile definitions for scripting system
target_compile_definitions(${CMAKE_PROJECT_NAME}_lib PRIVATE 
    ENABLE_SCRIPTING
    SCRIPTING_VERSION=1
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE 
    ENABLE_SCRIPTING
    SCRIPTING_VERSION=1
)
