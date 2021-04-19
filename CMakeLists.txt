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
  set(VT_DEVICE "CUSTOM_DEVICE")
endif()

if(VT_CODE_COVERAGE)
  include(CodeCoverageTarget)
  set(COV_EXCLUDE ${CMAKE_CURRENT_LIST_DIR}/src/platform/*)
  add_code_coverage_target(vt_core_test)
endif()

if(VT_DEVICE STREQUAL "MXCHIP_AZ3166")
	list(APPEND VT_LINK_LIBRARIES "stm32cubef4")
	list(APPEND VT_SOURCES "src/platform/vt_dsc_mxchip.c")
	list(APPEND VT_COMPILE_DEFINITIONS "STM32F4XX")
elseif(VT_DEVICE STREQUAL "STM_BL475EIOT01A")
	list(APPEND VT_LINK_LIBRARIES "stm32cubel4")
	list(APPEND VT_SOURCES "src/platform/vt_dsc_stm32.c")
	list(APPEND VT_COMPILE_DEFINITIONS "STM32L4XX")
elseif(VT_DEVICE STREQUAL "CUSTOM_DEVICE")
	list(APPEND VT_SOURCES "src/platform/vt_dsc_custom.c")
	message(WARNING "<CUSTOM_DEVICE> has been selected as device type, kindly make sure all functions in <src/platform/vt_dsc_custom.c> are implemented to avoid any runtime errors.")
else()
	message(FATAL_ERROR "Hardware for Verified Telemetry has not been appropriately defined. Valid options : <MXCHIP_AZ3166>, <STM_BL475EIOT01A> OR <CUSTOM_DEVICE>")
endif()

if(NOT WIN32)
  list(APPEND VT_LINK_LIBRARIES m)
endif()

# Add source to this project's executable.
list(APPEND VT_SOURCES
	"src/core/vt_sensor_initialize.c" 
	"src/core/vt_sensor_calibrate.c" 
	"src/core/vt_sensor_read.c" 
	"src/core/vt_fingerprint_calculate.c"
	"src/core/vt_fingerprint_evaluate.c"
	"src/core/vt_database_initialize.c"
	"src/core/vt_database_store.c"
	"src/core/vt_database_evaluate.c"
	"src/core/vt_database_fetch.c"
)

# Don't build middleware if unit testing enabled
if (NOT VT_UNIT_TESTING)
  list(APPEND VT_SOURCES
    "src/middleware/pnp_verified_telemetry.c" 
    "src/middleware/pnp_fallcurve_component.c"
    "src/middleware/pnp_middleware_helper.c"
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

target_compile_definitions(${TARGET} 
  PUBLIC 
    ${VT_COMPILE_DEFINITIONS}
)

target_include_directories(${TARGET}
  PUBLIC 
    inc/core
    inc/middleware
    inc/platform
    inc/core/internal
)

