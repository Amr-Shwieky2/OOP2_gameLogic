# Exceptions module CMakeLists.txt

# Source files - explicitly list all files
set(EXCEPTIONS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExceptionRecoverySystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameCheckpoint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceRetry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UserFriendlyErrors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RecoveryStrategyFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExceptionRecoveryDemo.cpp
    # Add any other source files here
)

# Header files
set(EXCEPTIONS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/Exceptions/Exception.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/GameExceptions.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/Logger.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/SafeResource.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/DebugUtils.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/ExceptionRecoverySystem.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/GameCheckpoint.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/ResourceRetry.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/UserFriendlyErrors.h
    ${CMAKE_SOURCE_DIR}/include/Exceptions/RecoveryStrategyFactory.h
    # Add any other header files here
)

# Add the source files to the parent scope variable
set(GAME_SOURCES ${GAME_SOURCES} ${EXCEPTIONS_SOURCES} PARENT_SCOPE)
set(GAME_HEADERS ${GAME_HEADERS} ${EXCEPTIONS_HEADERS} PARENT_SCOPE)