/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_API_H
#define _VT_CS_API_H

#include "vt_defs.h"
#include "vt_cs.h"
#include "vt_cs_config.h"
#include "vt_platform.h"

typedef struct VT_CURRENTSENSE_OBJECT_STRUCT
{
   VT_SENSOR_HANDLE* sensor_handle;
   VT_CURRENTSENSE_DATABASE fingerprintdb;
   VT_DEVICE_DRIVER* device_driver;
} VT_CURRENTSENSE_OBJECT;

typedef struct VT_CURRENTSENSE_DATABASE_FLATTENED
{
   VT_UCHAR num_signatures[10];
   VT_UCHAR avg_curr[10];
   VT_UCHAR lowest_sample_freq[10];
   VT_UCHAR sampling_freq[10 * VT_CS_MAX_SIGNATURES];
   VT_UCHAR signal_freq[10 * VT_CS_MAX_SIGNATURES];
   VT_UCHAR curr_diff[10 * VT_CS_MAX_SIGNATURES];
   VT_UCHAR duty_cycle[10 * VT_CS_MAX_SIGNATURES];
} VT_CURRENTSENSE_DATABASE_FLATTENED;

// Initialize
VT_VOID vt_currentsense_object_initialize(
   VT_CURRENTSENSE_OBJECT* cs_object,
   VT_DEVICE_DRIVER* device_driver, 
   VT_SENSOR_HANDLE* sensor_handle);

// Calibrate
VT_UINT vt_currentsense_object_sensor_calibrate(VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT8* confidence_metric);

// Re-Calibrate
VT_UINT vt_currentsense_object_sensor_recalibrate(VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT8* confidence_metric);

// Status
VT_VOID vt_currentsense_object_sensor_status(
   VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT* sensor_status, VT_UINT* sensor_drift);

// Sync Database
VT_VOID vt_currentsense_object_database_sync(
   VT_CURRENTSENSE_OBJECT* cs_object, VT_CURRENTSENSE_DATABASE_FLATTENED flattened_db);

// Fetch Database
VT_VOID vt_currentsense_object_database_fetch(
   VT_CURRENTSENSE_OBJECT* cs_object, VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db);


#endif
