# FindLua.cmake
# A simplified version that builds Lua if not found on the system

# First try to find system Lua
find_path(LUA_INCLUDE_DIR lua.h
  HINTS
    ENV LUA_DIR
  PATH_SUFFIXES include include/lua include/lua5.4 include/lua-5.4 include/lua5.3 include/lua-5.3
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
)

find_library(LUA_LIBRARY
  NAMES lua lua54 lua5.4 lua-5.4 lua53 lua5.3 lua-5.3 lua52 lua5.2 lua-5.2
  HINTS
    ENV LUA_DIR
  PATH_SUFFIXES lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /sw
    /opt/local
    /opt/csw
    /opt
)

# If not found, build Lua from source
if(NOT LUA_INCLUDE_DIR OR NOT LUA_LIBRARY)
  message(STATUS "Lua not found, building from source")
  
  # Create a simple Lua build
  file(WRITE ${CMAKE_BINARY_DIR}/lua_download.cmake "
    file(DOWNLOAD
      https://www.lua.org/ftp/lua-5.4.6.tar.gz
      ${CMAKE_BINARY_DIR}/lua-5.4.6.tar.gz
      SHOW_PROGRESS
    )
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/lua-5.4.6.tar.gz
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
  ")
  
  # Download and extract Lua
  execute_process(
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/lua_download.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  
  # Set source and include dirs
  set(LUA_SRC_DIR ${CMAKE_BINARY_DIR}/lua-5.4.6/src)
  set(LUA_INCLUDE_DIR ${LUA_SRC_DIR})
  
  # Collect all Lua source files except lua.c and luac.c
  file(GLOB LUA_SOURCES ${LUA_SRC_DIR}/*.c)
  list(REMOVE_ITEM LUA_SOURCES 
    ${LUA_SRC_DIR}/lua.c 
    ${LUA_SRC_DIR}/luac.c
  )
  
  # Create static Lua library
  add_library(lua_lib STATIC ${LUA_SOURCES})
  
  # Windows DLL definitions
  if(WIN32)
    target_compile_definitions(lua_lib PRIVATE LUA_BUILD_AS_DLL)
  endif()
  
  # Include directories
  target_include_directories(lua_lib PUBLIC ${LUA_INCLUDE_DIR})
  
  # Unix math library
  if(UNIX AND NOT APPLE)
    target_link_libraries(lua_lib PRIVATE m)
  endif()
  
  # Set variables for the caller
  set(LUA_LIBRARIES lua_lib)
  set(LUA_VERSION_STRING "5.4.6")
  set(LUA_FOUND TRUE)
else()
  # Extract version from the found library
  if(LUA_INCLUDE_DIR AND EXISTS "${LUA_INCLUDE_DIR}/lua.h")
    file(STRINGS "${LUA_INCLUDE_DIR}/lua.h" lua_version_str REGEX "^#define[ \t]+LUA_VERSION_[A-Z]+[ \t]+\".*\"")
    
    string(REGEX REPLACE ".*#define[ \t]+LUA_VERSION_MAJOR[ \t]+\"([0-9])\".*" "\\1" LUA_VERSION_MAJOR "${lua_version_str}")
    string(REGEX REPLACE ".*#define[ \t]+LUA_VERSION_MINOR[ \t]+\"([0-9])\".*" "\\1" LUA_VERSION_MINOR "${lua_version_str}")
    string(REGEX REPLACE ".*#define[ \t]+LUA_VERSION_RELEASE[ \t]+\"([0-9])\".*" "\\1" LUA_VERSION_PATCH "${lua_version_str}")
    
    set(LUA_VERSION_STRING "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_PATCH}")
    set(LUA_LIBRARIES ${LUA_LIBRARY})
    set(LUA_FOUND TRUE)
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua
  REQUIRED_VARS LUA_LIBRARIES LUA_INCLUDE_DIR
  VERSION_VAR LUA_VERSION_STRING
)

mark_as_advanced(
  LUA_INCLUDE_DIR
  LUA_LIBRARY
)