# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

# CMakeList.txt : CMake project for CMakeProject2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)


project (vt_core_test LANGUAGES C)
set(CMAKE_C_STANDARD 99)

include("${vt_SOURCE_DIR}/AddCMockaTest.cmake")

add_cmocka_test(vt_core_test SOURCES
                main.c
                test_vt_fingerprint.c
                test_vt_sensor.c
                test_vt_database.c
                COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS} ${NO_CLOBBERED_WARNING}
                LINK_LIBRARIES ${CMOCKA_LIBRARIES} verified_telemetry
                # include cmoka headers and private folder headers
                INCLUDE_DIRECTORIES ${CMOCKA_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src/core/ ${CMAKE_SOURCE_DIR}/inc/core/ ${CMAKE_SOURCE_DIR}/inc/platform/
                )

add_cmocka_test_environment(vt_core_test)