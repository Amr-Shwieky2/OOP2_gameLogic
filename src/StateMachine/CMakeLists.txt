# State Machine module
set(STATE_MACHINE_SOURCES
    StateMachineDemo.cpp
)

set(STATE_MACHINE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/StateMachine/StateMachine.h
    ${CMAKE_SOURCE_DIR}/include/StateMachine/PlayerStateMachine.h
    ${CMAKE_SOURCE_DIR}/include/StateMachine/StateMachineGuards.h
    ${CMAKE_SOURCE_DIR}/include/StateMachine/StateMachineDSL.h
    ${CMAKE_SOURCE_DIR}/include/StateMachine/StateMachineDiagram.h
    ${CMAKE_SOURCE_DIR}/include/StateMachine/StateMachineTests.h
    ${CMAKE_SOURCE_DIR}/include/StateMachine/ValidatedPlayerStateManager.h
)

# Add library target
add_library(StateMachine ${STATE_MACHINE_SOURCES} ${STATE_MACHINE_HEADERS})

# Include directories
target_include_directories(StateMachine 
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/include/StateMachine
)

# Make the StateMachine library available to the main executable
target_link_libraries(${CMAKE_PROJECT_NAME}_lib PRIVATE StateMachine)
