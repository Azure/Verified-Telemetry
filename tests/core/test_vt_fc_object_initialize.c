/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "vt_test_definitions.h"

#include "vt_fc_api.h"

#include "cmocka.h"

#define TEST_ADC_ID_RANDOM     0x23
#define TEST_TICK_VALUE_RANDOM 9542

static VT_ULONG vt_tick()
{
    return TEST_TICK_VALUE_RANDOM;
}

// vt_fallcurve_object_initialize()
static VT_VOID test_vt_fallcurve_object_initialize(VT_VOID** state)
{
    VT_FALLCURVE_OBJECT fc_object;
    VT_DEVICE_DRIVER device_driver;
    VT_SENSOR_HANDLE sensor_handle;

    device_driver.tick   = &vt_tick;
    sensor_handle.adc_id = TEST_ADC_ID_RANDOM;

    vt_fallcurve_object_initialize(&fc_object, &device_driver, &sensor_handle);

    assert_int_equal(fc_object.fingerprintdb.num_signatures, 0);
    assert_int_equal(fc_object.device_driver->tick(), TEST_TICK_VALUE_RANDOM);
    assert_int_equal(fc_object.sensor_handle->adc_id, TEST_ADC_ID_RANDOM);
}

VT_INT test_vt_fc_object_initialize()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_fallcurve_object_initialize),
    };

    return cmocka_run_group_tests_name("test_vt_fc_object_initialize", tests, NULL, NULL);
}
