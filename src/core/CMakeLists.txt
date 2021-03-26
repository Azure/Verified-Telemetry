# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10 FATAL_ERROR)

project (vt_core LANGUAGES C )
set(CMAKE_C_STANDARD 99)

add_library (
  vt_core
    ${CMAKE_CURRENT_LIST_DIR}/vt_sensor_initialize.c 
    ${CMAKE_CURRENT_LIST_DIR}/vt_sensor_calibrate.c 
    ${CMAKE_CURRENT_LIST_DIR}/vt_sensor_read.c 
    ${CMAKE_CURRENT_LIST_DIR}/vt_fingerprint_calculate.c
    ${CMAKE_CURRENT_LIST_DIR}/vt_fingerprint_evaluate.c
    ${CMAKE_CURRENT_LIST_DIR}/vt_database_initialize.c
    ${CMAKE_CURRENT_LIST_DIR}/vt_database_store.c
    ${CMAKE_CURRENT_LIST_DIR}/vt_database_evaluate.c
    ${CMAKE_CURRENT_LIST_DIR}/vt_database_clear.c
    ${CMAKE_CURRENT_LIST_DIR}/vt_database_fetch.c
)

target_include_directories (vt_core
  PUBLIC
  $<BUILD_INTERFACE:${vt_SOURCE_DIR}/inc/core>
  $<BUILD_INTERFACE:${vt_SOURCE_DIR}/inc/platform>
  $<INSTALL_INTERFACE:include/vt_core>
)

# include test internal headers
target_include_directories(vt_core
  PUBLIC
    $<BUILD_INTERFACE:${vt_SOURCE_DIR}/tests/core/inc>
)

# make sure that users can consume the project as a library.
add_library (vt::core ALIAS vt_core)