# Scripting module CMakeLists.txt

set(SCRIPTING_SOURCES
    ScriptManager.cpp
    LuaScriptEngine.cpp
    ScriptComponent.cpp
    ScriptBindingGenerator.cpp
    ScriptValidator.cpp
    ScriptDebugger.cpp
    ScriptPerformanceMonitor.cpp
    ScriptingDemo.cpp
    # Include the Lua stub implementation
    lua_stub/lua_stub.c
)

set(SCRIPTING_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Scripting/IScriptEngine.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/LuaScriptEngine.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptComponent.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptManager.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptBindingGenerator.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptValidator.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptDebugger.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptPerformanceMonitor.h
    ${CMAKE_SOURCE_DIR}/include/Scripting/ScriptingDemo.h
    # Include the Lua stub header
    lua_stub/lua_stub.h
)

# Define that we're using a stub implementation
add_compile_definitions(USE_LUA_STUB)

# Create the lua_stub directory if it doesn't exist yet
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lua_stub)

# Add the sources to the library
target_sources(${CMAKE_PROJECT_NAME}_lib PRIVATE ${SCRIPTING_SOURCES} ${SCRIPTING_HEADERS})

# Include our stub header for the library
target_include_directories(${CMAKE_PROJECT_NAME}_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lua_stub)

# Include our stub header for the executable
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lua_stub)

# Create scripts directory in source if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts)

# Create script directory in build folder
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/scripts
)

# Copy sample scripts to the build directory
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/scripts
    COMMENT "Copying scripts to build directory"
)

# Source groups for IDE organization
source_group("Source Files\\Scripting" FILES ${SCRIPTING_SOURCES})
source_group("Header Files\\Scripting" FILES ${SCRIPTING_HEADERS})
