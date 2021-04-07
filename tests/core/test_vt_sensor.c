/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "cmocka.h"

static void test_vt_sensor_initialize(void** state)
{
    (void)state;

}

static void test_vt_sensor_read_value(void** state)
{
    (void)state;

}

static void test_vt_sensor_read_fingerprint(void** state)
{
    (void)state;

}

static void test_vt_sensor_read_status(void** state)
{
    (void)state;

}

static void test_vt_sensor_calibrate(void** state)
{
    (void)state;

}

int __wrap__vt_dsc_adc_read(int* adc_controller, int adc_channel, int* value)
{
    return 0;
}

int test_vt_sensor()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_sensor_initialize),
        cmocka_unit_test(test_vt_sensor_read_value),
        cmocka_unit_test(test_vt_sensor_read_fingerprint),
        cmocka_unit_test(test_vt_sensor_read_status),
        cmocka_unit_test(test_vt_sensor_calibrate),
    };

    return cmocka_run_group_tests_name("vt_core_sensor", tests, NULL, NULL);
}