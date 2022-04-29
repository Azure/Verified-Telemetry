# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET verified_telemetry_freertos)

add_library(${TARGET}
    "FreeRTOS_verified_telemetry.c" 
    "FreeRTOS_vt_fallcurve_component.c"
    "FreeRTOS_vt_currentsense_component.c"
)

add_library(az::iot::vt 
    ALIAS ${TARGET}
)

target_link_libraries(${TARGET} 
    PUBLIC
        az::iot::vt::core

    PRIVATE
        az::iot_middleware::freertos
)

target_include_directories(${TARGET}
    PUBLIC
        ${VT_BASE_DIR}/inc/middleware/freertos_middleware

    PRIVATE
        ${CONFIG_DIRECTORY}
        ${FREERTOS_PORT_DIRECTORY}
        ${FREERTOS_DIRECTORY}/FreeRTOS/Source/include
)
