# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project (vt_middleware LANGUAGES C )
set(CMAKE_C_STANDARD 99)

add_library (
  vt_middleware
    ${CMAKE_CURRENT_LIST_DIR}/pnp_fallcurve_component.c 
    ${CMAKE_CURRENT_LIST_DIR}/pnp_middleware_helper.c 
    ${CMAKE_CURRENT_LIST_DIR}/pnp_verified_telemetry.c 
)

target_include_directories (vt_middleware
  PUBLIC
  $<BUILD_INTERFACE:${vt_SOURCE_DIR}/inc/middleware>
  $<BUILD_INTERFACE:${vt_SOURCE_DIR}/inc/core>
  $<INSTALL_INTERFACE:include/vt_middleware>
)

# make sure that users can consume the project as a library.
add_library (vt::middleware ALIAS vt_middleware)