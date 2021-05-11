/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_database.h"

VT_VOID vt_currentsense_object_initialize(VT_CURRENTSENSE_OBJECT* cs_object, VT_DEVICE_DRIVER* device_driver, VT_SENSOR_HANDLE* sensor_handle)
{
   cs_object->sensor_handle = sensor_handle;

   cs_object->device_driver = device_driver;

   cs_reset_db(cs_object);
}