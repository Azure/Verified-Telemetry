# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

set(CMAKE_C_STANDARD 99)
#set(VT_MIDDLEWARE_RTOS "FREERTOS" CACHE STRING "Set VT Middleware to FreeRTOS")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(VT_UNIT_TESTING "Build unit test projects" ON)
option(VT_CODE_COVERAGE "Run code coverage" OFF)

project(vt LANGUAGES C)

set(VT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# include core
add_subdirectory(src/core)

# default for Unit testing with cmocka is OFF
if (VT_UNIT_TESTING)
    add_subdirectory(tests)
endif()

# default for Code coverage is OFF.
if(VT_CODE_COVERAGE)
    include(CodeCoverageTarget)
    add_code_coverage_target(vt_core_test)
endif()

# include the appropriate middleware
if(NOT VT_UNIT_TESTING)

    if(VT_MIDDLEWARE_RTOS STREQUAL "AZURE_RTOS")
        add_subdirectory(src/middleware/azure_rtos_middleware)

    elseif(VT_MIDDLEWARE_RTOS STREQUAL "FREERTOS")
        add_subdirectory(src/middleware/freertos_middleware)

    else()
        message(FATAL_ERROR, "VT_MIDDLEWARE_RTOS not specified. Must be one of 'AZURE_RTOS' or 'FREERTOS'")

    endif()

endif()
