/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_fc_api.h"
#include "vt_fc_database.h"

VT_VOID vt_fallcurve_object_initialize(
    VT_FALLCURVE_OBJECT* fc_object, VT_DEVICE_DRIVER* device_driver, VT_SENSOR_HANDLE* sensor_handle)
{
    fc_object->sensor_handle = sensor_handle;

    fc_object->device_driver = device_driver;

    fc_reset_db(fc_object);
}
