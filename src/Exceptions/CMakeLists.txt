# Exceptions module CMakeLists.txt

# Source files - explicitly list all files
set(EXCEPTIONS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
    # Add any other source files here
)

# Header files
set(EXCEPTIONS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Exceptions/Exception.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/GameExceptions.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/Logger.h
    # Add any other header files here
)

# Add the source files to the parent scope variable
set(GAME_SOURCES ${GAME_SOURCES} ${EXCEPTIONS_SOURCES} PARENT_SCOPE)
set(GAME_HEADERS ${GAME_HEADERS} ${EXCEPTIONS_HEADERS} PARENT_SCOPE)