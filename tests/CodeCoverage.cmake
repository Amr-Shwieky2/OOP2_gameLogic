# Copyright (c) 2012 - 2017, Lars Bilke
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Simplified version for our project - requires lcov and genhtml

# Check prereqs
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if(NOT LCOV_PATH)
    message(FATAL_ERROR "lcov not found! Aborting...")
endif()

if(NOT GENHTML_PATH)
    message(FATAL_ERROR "genhtml not found! Aborting...")
endif()

# Defines a target for running and collection code coverage information
# Builds dependencies, runs the given executable and outputs reports.
function(SETUP_TARGET_FOR_COVERAGE_LCOV)

    set(options "")
    set(oneValueArgs NAME)
    set(multiValueArgs EXECUTABLE DEPENDENCIES EXCLUDE)
    cmake_parse_arguments(Coverage "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT Coverage_EXECUTABLE)
        message(FATAL_ERROR "No executable specified")
    endif()

    # Setup target
    add_custom_target(${Coverage_NAME}
        # Cleanup lcov
        COMMAND ${LCOV_PATH} --directory . --zerocounters

        # Run tests
        COMMAND ${Coverage_EXECUTABLE}

        # Capturing lcov counters and generating report
        COMMAND ${LCOV_PATH} --directory . --capture --output-file ${Coverage_NAME}.info
        COMMAND ${LCOV_PATH} --remove ${Coverage_NAME}.info '/usr/*' '*/external/*' '*/tests/*' --output-file ${Coverage_NAME}.info.cleaned
        COMMAND ${GENHTML_PATH} -o ${Coverage_NAME} ${Coverage_NAME}.info.cleaned
        COMMAND ${CMAKE_COMMAND} -E remove ${Coverage_NAME}.info ${Coverage_NAME}.info.cleaned

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${Coverage_DEPENDENCIES}
        COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
    )

    # Show info where to find the report
    add_custom_command(TARGET ${Coverage_NAME} POST_BUILD
        COMMAND ;
        COMMENT "Open ./${Coverage_NAME}/index.html in your browser to view the coverage report."
    )

endfunction()

function(APPEND_COVERAGE_COMPILER_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage" PARENT_SCOPE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage" PARENT_SCOPE)
    message(STATUS "Appending code coverage compiler flags: -g -O0 -fprofile-arcs -ftest-coverage")
endfunction()