/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_API_H
#define _VT_CS_API_H

#include "vt_cs_config.h"
#include "vt_defs.h"
#include "vt_platform.h"

typedef struct VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER_STRUCT
{
    VT_FLOAT sampling_frequency;
    VT_UINT sample_length;
    VT_UINT num_datapoints;
    VT_FLOAT current_measured[VT_CS_SAMPLE_LENGTH];
} VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER;

typedef struct VT_CURRENTSENSE_RAW_SIGNATURES_READER_STRUCT
{
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER raw_signatures_shared_buffer[VT_CS_MAX_SIGNATURES];
    VT_UINT num_raw_signatures;
    VT_FLOAT adc_read_buffer[VT_CS_SAMPLE_LENGTH];
    VT_FLOAT adc_read_sampling_frequency;
    VT_BOOL raw_signature_ongoing_collection;
} VT_CURRENTSENSE_RAW_SIGNATURES_READER;

typedef struct VT_CURRENTSENSE_TEMPLATE_SIGNATURE_FEATURES_STRUCT
{
    VT_FLOAT sampling_freq;
    VT_FLOAT signal_freq;
    VT_FLOAT curr_diff;
    VT_FLOAT duty_cycle;
} VT_CURRENTSENSE_TEMPLATE_SIGNATURE_FEATURES;

typedef struct VT_CURRENTSENSE_DATABASE_STRUCT
{
    VT_UINT num_signatures;
    VT_FLOAT avg_curr;
    VT_FLOAT lowest_sample_freq;
    VT_CURRENTSENSE_TEMPLATE_SIGNATURE_FEATURES db[VT_CS_MAX_SIGNATURES];
} VT_CURRENTSENSE_DATABASE;

typedef struct VT_CURRENTSENSE_OBJECT_STRUCT
{
    VT_SENSOR_HANDLE* sensor_handle;
    VT_CURRENTSENSE_DATABASE fingerprintdb;
    VT_DEVICE_DRIVER* device_driver;
    VT_CURRENTSENSE_RAW_SIGNATURES_READER* raw_signatures_reader;
    VT_BOOL raw_signatures_reader_initialized;
    VT_UINT8 mode;
    VT_UINT8 sensor_status;
    VT_UINT8 sensor_drift;
    VT_UINT8 template_confidence_metric;
    VT_UINT8 db_updated;
} VT_CURRENTSENSE_OBJECT;

typedef struct VT_CURRENTSENSE_DATABASE_FLATTENED
{
    VT_UCHAR num_signatures[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR avg_curr[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR lowest_sample_freq[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR sampling_freq[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
    VT_UCHAR signal_freq[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
    VT_UCHAR curr_diff[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
    VT_UCHAR duty_cycle[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
} VT_CURRENTSENSE_DATABASE_FLATTENED;

// Initialize
VT_UINT vt_currentsense_object_initialize(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    VT_CHAR* raw_signatures_buffer,
    VT_UINT raw_signatures_buffer_size);

// Set mode to calibrate
VT_VOID vt_currentsense_object_sensor_calibrate(VT_CURRENTSENSE_OBJECT* cs_object);

// Set mode to re-calibrate
VT_VOID vt_currentsense_object_sensor_recalibrate(VT_CURRENTSENSE_OBJECT* cs_object);

// Fetch Status
VT_VOID vt_currentsense_object_sensor_fetch_status(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT* sensor_status, VT_UINT* sensor_drift);

// Start reading current signature
VT_VOID vt_currentsense_object_signature_read(VT_CURRENTSENSE_OBJECT* cs_object);

// Stop reading current signature and process it
VT_VOID vt_currentsense_object_signature_process(VT_CURRENTSENSE_OBJECT* cs_object);

// Sync Database
VT_VOID vt_currentsense_object_database_sync(VT_CURRENTSENSE_OBJECT* cs_object, VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db);

// Fetch Database and template confidence
VT_VOID vt_currentsense_object_database_fetch(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db, VT_BOOL* db_updated, VT_UINT* template_confidence_metric);

#endif
