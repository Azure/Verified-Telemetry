/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"

VT_VOID vt_currentsense_object_sensor_calibrate(VT_CURRENTSENSE_OBJECT* cs_object)
{   cs_object->Calibration_Done_Count=0;
    cs_object->Initial_Calibration_Done=false;
    cs_object->mode = VT_MODE_CALIBRATE;
}

VT_VOID vt_currentsense_object_sensor_recalibrate(VT_CURRENTSENSE_OBJECT* cs_object)
{
    cs_object->mode = VT_MODE_RECALIBRATE;
}

VT_VOID vt_currentsense_object_sensor_fetch_status(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT* sensor_status, VT_UINT* sensor_drift)
{
    *sensor_status = cs_object->sensor_status;
    *sensor_drift  = cs_object->sensor_drift;
}
