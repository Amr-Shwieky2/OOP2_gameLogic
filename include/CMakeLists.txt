# Add all header files recursively
file(GLOB_RECURSE MY_HEADER_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MY_HEADER_FILES})

# Include root and subdirectories so that legacy include paths continue to work
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Core
    ${CMAKE_CURRENT_LIST_DIR}/Components
    ${CMAKE_CURRENT_LIST_DIR}/Entities/Player
    ${CMAKE_CURRENT_LIST_DIR}/Entities/Enemies
    ${CMAKE_CURRENT_LIST_DIR}/Entities/Environment
    ${CMAKE_CURRENT_LIST_DIR}/Entities/Collectibles
    ${CMAKE_CURRENT_LIST_DIR}/Entities/Combat
    ${CMAKE_CURRENT_LIST_DIR}/Systems/Managers
    ${CMAKE_CURRENT_LIST_DIR}/Systems/Events
    ${CMAKE_CURRENT_LIST_DIR}/Systems/Rendering
    ${CMAKE_CURRENT_LIST_DIR}/Systems/Collision
    ${CMAKE_CURRENT_LIST_DIR}/AI
    ${CMAKE_CURRENT_LIST_DIR}/States
    ${CMAKE_CURRENT_LIST_DIR}/UI
    ${CMAKE_CURRENT_LIST_DIR}/Utilities
    ${CMAKE_CURRENT_LIST_DIR}/Exceptions
    ${CMAKE_CURRENT_LIST_DIR}/Game
)
