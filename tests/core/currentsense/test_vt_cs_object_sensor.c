/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "test_vt_cs_definitions.h"

#include "vt_cs_api.h"

#include "cmocka.h"

// vt_currentsense_object_sensor_calibrate()
static VT_VOID test_vt_currentsense_object_sensor_calibrate(VT_VOID** state)
{
    VT_CURRENTSENSE_OBJECT cs_object;

    cs_object.mode = 0x00;
    vt_currentsense_object_sensor_calibrate(&cs_object);
    assert_int_equal(cs_object.mode, VT_MODE_CALIBRATE);
}

// vt_currentsense_object_sensor_recalibrate()
static VT_VOID test_vt_currentsense_object_sensor_recalibrate(VT_VOID** state)
{
    VT_CURRENTSENSE_OBJECT cs_object;

    cs_object.mode = 0x00;
    vt_currentsense_object_sensor_recalibrate(&cs_object);
    assert_int_equal(cs_object.mode, VT_MODE_RECALIBRATE);
}

// vt_currentsense_object_sensor_fetch_status()
static VT_VOID test_vt_currentsense_object_sensor_fetch_status(VT_VOID** state)
{
    VT_CURRENTSENSE_OBJECT cs_object;
    VT_UINT sensor_status = 10;
    VT_UINT sensor_drift  = 10;

    cs_object.sensor_status = VT_SIGNATURE_MATCHING;
    cs_object.sensor_drift  = 56;
    vt_currentsense_object_sensor_fetch_status(&cs_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_MATCHING);
    assert_int_equal(sensor_drift, 56);
}

VT_INT test_vt_cs_object_sensor()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_currentsense_object_sensor_calibrate),
        cmocka_unit_test(test_vt_currentsense_object_sensor_recalibrate),
        cmocka_unit_test(test_vt_currentsense_object_sensor_fetch_status),
    };

    return cmocka_run_group_tests_name("test_vt_cs_object_sensor", tests, NULL, NULL);
}
