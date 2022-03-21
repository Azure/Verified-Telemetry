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
    VT_UINT num_adc_buffer_datapoints_iterated;
    VT_FLOAT current_measured[VT_CS_SAMPLE_LENGTH];
} VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER;

typedef struct VT_CURRENTSENSE_RAW_SIGNATURES_READER_STRUCT
{
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER repeating_raw_signatures[VT_CS_MAX_SIGNATURES];
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER non_repeating_raw_signature;
    VT_UINT num_repeating_raw_signatures;
    VT_FLOAT adc_read_buffer[VT_CS_SAMPLE_LENGTH];
    VT_FLOAT adc_read_sampling_frequency;
    VT_BOOL repeating_raw_signature_ongoing_collection;
    VT_BOOL repeating_raw_signature_buffers_filled;
    VT_BOOL non_repeating_raw_signature_stop_collection;
} VT_CURRENTSENSE_RAW_SIGNATURES_READER;

typedef struct VT_CURRENTSENSE_NON_REPEATING_SIGNATURE_TEMPLATE_STRUCT
{
    VT_FLOAT avg_curr_on;
    VT_FLOAT avg_curr_off;
    VT_UINT8 non_repeating_sensor_status;
    VT_BOOL non_repeating_valid;
    VT_UINT avg_curr_on_off_repeatable_count;
} VT_CURRENTSENSE_NON_REPEATING_SIGNATURE_TEMPLATE;

typedef struct VT_CURRENTSENSE_REPEATING_SIGNATURE_FEATURE_VECTOR_STRUCT
{
    VT_FLOAT sampling_freq;
    VT_FLOAT signature_freq;
    VT_FLOAT relative_curr_draw;
    VT_FLOAT duty_cycle;
    VT_UINT sampling_freq_repeatable_count;
    VT_UINT signature_freq_and_duty_cycle_repeatable_count;
    VT_UINT current_dif_repeatable_count;
    VT_BOOL signature_freq_and_duty_cycle_valid;
    VT_BOOL current_dif_valid;
    //VT_BOOL repeating_signature_specific_status;//have status for each signature, so that one freq that changes dosent currupt the output.
    //and we can see the unrelaiable freq.
} VT_CURRENTSENSE_REPEATING_SIGNATURE_FEATURE_VECTOR;

typedef struct VT_CURRENTSENSE_SIGNATURE_REPEATABILITY_DATA_STRUCT
{
    VT_FLOAT sampling_freq_found;
    VT_UINT sampling_freq_repeatable_count;
    VT_UINT signature_freq_and_duty_cycle_repeatable_count;
    VT_UINT current_dif_repeatable_count;
    //VT_BOOL repeating_signature_specific_status;//have status for each signature, so that one freq that changes dosent currupt the output.
    //and we can see the unrelaiable freq.
} VT_CURRENTSENSE_SIGNATURE_REPEATABILITY_DATA;

typedef struct VT_CURRENTSENSE_REPEATING_SIGNATURES_TEMPLATE_STRUCT
{
    VT_UINT num_signatures;
    VT_UINT multisampling_num_signatures;
    VT_FLOAT offset_current;
    VT_FLOAT lowest_sample_freq;
    VT_BOOL repeating_valid;//not required now
    VT_BOOL repeating_sensor_status;//not req, as now the status would be linked to each signature, insted of whole repeating 
    VT_CURRENTSENSE_REPEATING_SIGNATURE_FEATURE_VECTOR signatures[VT_CS_MAX_SIGNATURES];
    VT_UINT Num_Sampling_Freq_In_Repeatability_Data_Array;
    VT_CURRENTSENSE_REPEATING_SIGNATURE_FEATURE_VECTOR Multisampling_Signatures_With_Repeatability_Data_Array[VT_CS_MULTISAMPLING_MAX_SIGNATURES];
    //VT_CURRENTSENSE_SIGNATURE_REPEATABILITY_DATA Signature_Repeatability_Data_Array[];
    // Sampling freq found || Sampling repetable || Signal & duty repetable || Curr dif repetable

} VT_CURRENTSENSE_REPEATING_SIGNATURES_TEMPLATE;
//THIS IS UNION HENCE NON-REP AND REP WILL CHANGE EACH OTHERS VALUE MAKE THIS A STRUCT THEN THERE WOULD BE NO PROBLEM
//THIS IS WHY NUM SIG GETS A VERY LARGE VALUE 
typedef struct VT_CURRENTSENSE_SIGNATURES_TEMPLATE_STRUCT {

    /* Non-Repeating Signature Template */
    VT_CURRENTSENSE_NON_REPEATING_SIGNATURE_TEMPLATE non_repeating_signature;

    /* Repeating Signatures Template */
    VT_CURRENTSENSE_REPEATING_SIGNATURES_TEMPLATE repeating_signatures;
}VT_CURRENTSENSE_SIGNATURES_TEMPLATE;

typedef struct VT_CURRENTSENSE_DATABASE_STRUCT
{
    VT_UINT template_type;
    VT_CURRENTSENSE_SIGNATURES_TEMPLATE template;
} VT_CURRENTSENSE_DATABASE;

typedef struct VT_CURRENTSENSE_OBJECT_STRUCT
{
    VT_SENSOR_HANDLE* sensor_handle;
    VT_CURRENTSENSE_DATABASE fingerprintdb;
    VT_DEVICE_DRIVER* device_driver;
    VT_CURRENTSENSE_RAW_SIGNATURES_READER* raw_signatures_reader;
    VT_BOOL raw_signatures_reader_initialized;
    VT_UINT8 mode;
    VT_UINT8 MultiCalibration_Count;
    VT_UINT8 Calibration_Done_Count;
    VT_BOOL Initial_Calibration_Done;
    VT_UINT8 sensor_status;
    VT_UINT8 sensor_drift;
    VT_UINT8 template_confidence_metric;
    VT_UINT8 db_updated;
    VT_BOOL hist_array[VT_HISTORY_ARRAY_LENGTH];
} VT_CURRENTSENSE_OBJECT;

typedef struct VT_CURRENTSENSE_DATABASE_FLATTENED
{
    VT_UCHAR template_type[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR non_repeating_signature_avg_curr_on[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR non_repeating_signature_avg_curr_off[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR repeating_signature_num_signatures[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR repeating_signature_offset_curr[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR repeating_signature_lowest_sample_freq[VT_CHARACTERS_IN_A_NUMBER];
    VT_UCHAR repeating_signature_sampling_freq[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
    VT_UCHAR repeating_signature_freq[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
    VT_UCHAR repeating_signature_relative_curr_draw[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
    VT_UCHAR repeating_signature_duty_cycle[VT_CHARACTERS_IN_A_NUMBER * VT_CS_MAX_SIGNATURES];
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
VT_VOID vt_currentsense_object_database_fetch(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db,
    VT_BOOL* db_updated,
    VT_UINT* template_confidence_metric);

#endif
