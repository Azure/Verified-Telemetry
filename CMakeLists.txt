# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD 99)

option(UNIT_TESTING "Build unit test projects" ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

project(vt LANGUAGES C)
set(TARGET verified_telemetry)
enable_testing ()

# default for Unit testing with cmocka is OFF, however, this will be ON on CI and tests must
# pass before committing changes
if (UNIT_TESTING)
  # make generate step fail if cmocka dependency is not found
  find_package(cmocka CONFIG REQUIRED)

  # When using VCPKG, cmocka lib name is set different than when it is globally installed
  if(DEFINED ENV{VCPKG_ROOT} OR DEFINED ENV{VCPKG_INSTALLATION_ROOT})
    set(CMOCKA_LIB ${CMOCKA_LIBRARIES})
  else()
    set(CMOCKA_LIB cmocka::cmocka)
  endif()

  # for gcc, we need to add no-clobbered compile opt to avoid warning about set-jump function
  set(NO_CLOBBERED_WARNING "")
  if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(NO_CLOBBERED_WARNING "-Wno-clobbered")
  endif()

  add_subdirectory(tests/core)

  set(VT_DEVICE "CUSTOM_DEVICE")

else ()
    # Add source to this project's executable.
  list(APPEND VT_SOURCES
    "src/middleware/pnp_verified_telemetry.c" 
    "src/middleware/pnp_fallcurve_component.c"
    "src/middleware/pnp_middleware_helper.c"
  )
  list(APPEND VT_LINK_LIBRARIES "app_common")

  list(APPEND VT_LINK_LIBRARIES m)
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
	message(WARNING  "<CUSTOM_DEVICE> has been selected as device type, kindly make sure all functions in <src/platform/vt_dsc_custom.c> are implemented to avoid any runtime errors.")
else()
	message(FATAL_ERROR "Hardware for Verified Telemetry has not been appropriately defined. Valid options : <MXCHIP_AZ3166> , <STM_BL475EIOT01A> OR <CUSTOM_DEVICE>")
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
	"src/core/vt_database_clear.c"
	"src/core/vt_database_fetch.c"
	)

add_library(${TARGET} OBJECT ${VT_SOURCES})
target_link_libraries(${TARGET} PUBLIC ${VT_LINK_LIBRARIES})
target_compile_definitions(${TARGET} PUBLIC ${VT_COMPILE_DEFINITIONS})

target_include_directories(${TARGET}
    PUBLIC 
        .
        inc/core
        inc/middleware
        inc/platform
)
