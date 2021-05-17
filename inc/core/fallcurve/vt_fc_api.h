/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_FC_API_H
#define _VT_FC_API_H

#include "vt_defs.h"
#include "vt_fc.h"
#include "vt_fc_config.h"
#include "vt_platform.h"

typedef struct VT_FALLCURVE_OBJECT_STRUCT
{
    VT_SENSOR_HANDLE* sensor_handle;
    VT_FALLCURVE_DATABASE fingerprintdb;
    VT_DEVICE_DRIVER* device_driver;
} VT_FALLCURVE_OBJECT;

typedef struct VT_FALLCURVE_DATABASE_FLATTENED_STRUCT
{
    VT_UCHAR num_signatures[10];
    VT_UCHAR sampling_interval_us[10 * VT_FC_MAX_SIGNATURES];
    VT_UCHAR falltime[10 * VT_FC_MAX_SIGNATURES];
    VT_UCHAR pearson_coeff[10 * VT_FC_MAX_SIGNATURES];
} VT_FALLCURVE_DATABASE_FLATTENED;

// Initialize
VT_VOID vt_fallcurve_object_initialize(
    VT_FALLCURVE_OBJECT* fc_object, VT_DEVICE_DRIVER* device_driver, VT_SENSOR_HANDLE* sensor_handle);

// Calibrate
VT_UINT vt_fallcurve_object_sensor_calibrate(VT_FALLCURVE_OBJECT* fc_object, VT_UINT8* confidence_metric);

// Re-Calibrate
VT_UINT vt_fallcurve_object_sensor_recalibrate(VT_FALLCURVE_OBJECT* fc_object, VT_UINT8* confidence_metric);

// Status
VT_VOID vt_fallcurve_object_sensor_status(VT_FALLCURVE_OBJECT* fc_object, VT_UINT* sensor_status, VT_UINT* sensor_drift);

// Sync Database
VT_VOID vt_fallcurve_object_database_sync(VT_FALLCURVE_OBJECT* fc_object, VT_FALLCURVE_DATABASE_FLATTENED flattened_db);

// Fetch Database
VT_VOID vt_fallcurve_object_database_fetch(VT_FALLCURVE_OBJECT* fc_object, VT_FALLCURVE_DATABASE_FLATTENED* flattened_db);

#endif
