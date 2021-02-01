# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

# CMakeList.txt : CMake project for CMakeProject2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(TARGET verified_telemetry)


if(VT_DEVICE STREQUAL "MXCHIP_AZ3166")
	list(APPEND VT_LINK_LIBRARIES "stm32cubef4")
	list(APPEND VT_SOURCES "src/platform/vt_dsc_mxchip.c")
	list(APPEND VT_COMPILE_DEFINITIONS "MXCHIP_AZ3166")
elseif(VT_DEVICE STREQUAL "STM_BL475EIOT01A")
	list(APPEND VT_LINK_LIBRARIES "stm32cubel4")
	list(APPEND VT_SOURCES "src/platform/vt_dsc_stm32.c")
	list(APPEND VT_COMPILE_DEFINITIONS "STM_BL475EIOT01A")
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

	"src/middleware/pnp_verified_telemetry.c" 
	"src/middleware/pnp_fallcurve_component.c"
	)

list(APPEND VT_LINK_LIBRARIES "app_common")

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

target_link_libraries(${TARGET} PRIVATE m) 
