# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET verified_telemetry_azure_rtos)

add_library(${TARGET}
    "nx_verified_telemetry.c" 
    "nx_vt_fallcurve_component.c"
    "nx_vt_currentsense_component.c"
    "nx_vt_middleware_helper.c"
)

add_library(az::iot::vt
    ALIAS ${TARGET}
)

target_link_libraries(${TARGET} 
    PUBLIC
        az::iot::vt::core 

    PRIVATE
        azrtos::netxduo
)

target_include_directories(${TARGET}
    PUBLIC
        ${VT_BASE_DIR}/inc/middleware/azure_rtos_middleware
)
