# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
set(CMAKE_C_STANDARD 99)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(VT_UNIT_TESTING "Build unit test projects" OFF)
option(VT_CODE_COVERAGE "Run code coverage" OFF)

project(vt LANGUAGES C)

# default for Unit testing with cmocka is OFF, however, this will be ON on CI and tests must pass before committing changes
if (VT_UNIT_TESTING)
  add_subdirectory(tests)
endif()

if(VT_CODE_COVERAGE)
  include(CodeCoverageTarget)
  add_code_coverage_target(vt_core_test)
endif()

if(NOT WIN32)
  list(APPEND VT_LINK_LIBRARIES m)
endif()

# Add source to this project's executable.
list(APPEND VT_SOURCES
  "src/core/fallcurve/vt_fc_object_database_fetch.c"
  "src/core/fallcurve/vt_fc_object_database_sync.c"
  "src/core/fallcurve/vt_fc_object_initialize.c"
  "src/core/fallcurve/vt_fc_object_sensor_calibrate.c"
  "src/core/fallcurve/vt_fc_object_sensor_status.c"
  "src/core/fallcurve/internal/vt_fc_database_fetch.c"
  "src/core/fallcurve/internal/vt_fc_database_reset.c"
  "src/core/fallcurve/internal/vt_fc_database_store.c"
  "src/core/fallcurve/internal/vt_fc_read.c"
  "src/core/fallcurve/internal/vt_fc_signature_collection_settings.c"
  "src/core/fallcurve/internal/vt_fc_signature_compute.c"
  "src/core/fallcurve/internal/vt_fc_signature_evaluate.c"
)

# Don't build middleware if unit testing enabled
if (NOT VT_UNIT_TESTING)
  list(APPEND VT_SOURCES
    "src/middleware/nx_verified_telemetry.c" 
    "src/middleware/nx_vt_fallcurve_component.c"
    "src/middleware/nx_vt_middleware_helper.c"
  )
endif()

set(TARGET verified_telemetry)

add_library(${TARGET} 
  ${VT_SOURCES}
)

add_library(az:iot:vt ALIAS verified_telemetry)

target_link_libraries(${TARGET} 
  PUBLIC 
    ${VT_LINK_LIBRARIES}
)
if (NOT VT_UNIT_TESTING)
  target_link_libraries(${TARGET} 
  PUBLIC 
    azrtos::netxduo
  )  
endif()

target_compile_definitions(${TARGET} 
  PUBLIC 
    ${VT_COMPILE_DEFINITIONS}
)

target_include_directories(${TARGET}
  PUBLIC 
    inc/core
    inc/middleware
    inc/platform
    inc/core/fallcurve
    inc/core/fallcurve/config
  PRIVATE
    inc/core/fallcurve/internal
)

