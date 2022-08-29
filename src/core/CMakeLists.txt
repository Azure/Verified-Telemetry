# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET verified_telemetry_core)

add_library(${TARGET}
    "fallcurve/vt_fc_object_database_fetch.c"
    "fallcurve/vt_fc_object_database_sync.c"
    "fallcurve/vt_fc_object_initialize.c"
    "fallcurve/vt_fc_object_sensor_calibrate.c"
    "fallcurve/vt_fc_object_sensor_status.c"
    "fallcurve/internal/vt_fc_database_fetch.c"
    "fallcurve/internal/vt_fc_database_reset.c"
    "fallcurve/internal/vt_fc_database_store.c"
    "fallcurve/internal/vt_fc_read.c"
    "fallcurve/internal/vt_fc_signature_collection_settings.c"
    "fallcurve/internal/vt_fc_signature_compute.c"
    "fallcurve/internal/vt_fc_signature_evaluate.c"

    "currentsense/vt_cs_object_database_fetch.c"
    "currentsense/vt_cs_object_database_sync.c"
    "currentsense/vt_cs_object_initialize.c"
    "currentsense/vt_cs_object_sensor.c"
    "currentsense/vt_cs_object_signature.c"
    "currentsense/internal/vt_cs_calibrate_compute_collection_settings.c"
    "currentsense/internal/vt_cs_calibrate_sensor.c"
    "currentsense/internal/vt_cs_database_fetch.c"
    "currentsense/internal/vt_cs_database_reset.c"
    "currentsense/internal/vt_cs_database_store.c"
    "currentsense/internal/vt_cs_fft.c"
    
    "currentsense/internal/vt_cs_raw_signature_read.c"
    "currentsense/internal/vt_cs_sensor_status_compute.c"
    "currentsense/internal/vt_cs_signature_features_compute.c"
    "currentsense/internal/vt_cs_signature_features_evaluate.c"
    "currentsense/internal/kmeans1.c"
)

add_library(az::iot::vt::core 
    ALIAS ${TARGET}
)

if(NOT WIN32)
    list(APPEND VT_LINK_LIBRARIES m)
endif()

target_link_libraries(${TARGET} 
    PUBLIC 
      ${VT_LINK_LIBRARIES}
)

target_include_directories(${TARGET}
    PUBLIC 
        ${VT_BASE_DIR}/inc/platform
        ${VT_BASE_DIR}/inc/core
        ${VT_BASE_DIR}/inc/core/fallcurve
        ${VT_BASE_DIR}/inc/core/fallcurve/config
        ${VT_BASE_DIR}/inc/core/currentsense
        ${VT_BASE_DIR}/inc/core/currentsense/config

    PRIVATE
        ${VT_BASE_DIR}/inc/core/fallcurve/internal
        ${VT_BASE_DIR}/inc/core/currentsense/internal
)
