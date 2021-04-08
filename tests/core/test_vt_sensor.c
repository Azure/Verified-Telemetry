/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"

#include "vt_api.h"
#include "vt_dsc.h"
#include "vt_port.h"
#include "vt_sensor.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "cmocka.h"

VT_SENSOR sensor;

static int vt_sensor_set(void** state)
{
    sensor.vt_sensor_name        = strdup("set_sensor");
    sensor.vt_gpio_port          = NULL;
    sensor.vt_gpio_pin           = 9;
    sensor.vt_adc_controller     = NULL;
    sensor.vt_adc_channel        = 3;
    sensor.vt_timer              = NULL;
    sensor.vt_sampling_frequency = 1;

    return 0;
}

static void test_vt_sensor_initialize(void** state)
{
    (void)state;

    vt_sensor_initialize(&sensor, "test_sensor", NULL, 7, NULL, 2, NULL);

    assert_string_equal(sensor.vt_sensor_name, "test_sensor");
    assert_ptr_equal(sensor.vt_gpio_port, NULL);
    assert_int_equal(sensor.vt_gpio_pin, 7);
    assert_ptr_equal(sensor.vt_adc_controller, NULL);
    assert_int_equal(sensor.vt_adc_channel, 2);
    assert_ptr_equal(sensor.vt_timer, NULL);
}

static void test_vt_sensor_read_value(void** state)
{
    (void)state;

    uint32_t value;

    expect_value(__wrap__vt_dsc_adc_read, adc_controller, NULL);
    expect_value(__wrap__vt_dsc_adc_read, adc_channel, 3);

    will_return(__wrap__vt_dsc_adc_read, 23);

    vt_sensor_read_value(&sensor, &value);
    assert_int_equal(value, 23);
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

int __wrap__vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* adc_controller, ADC_CHANNEL_TYPEDEF adc_channel, uint32_t* value)
{
    check_expected(adc_controller);
    check_expected(adc_channel);

    *value = (uint32_t)mock();

    return VT_PLATFORM_SUCCESS;
}

int test_vt_sensor()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_vt_sensor_initialize, vt_sensor_set, vt_sensor_set),
        cmocka_unit_test_setup(test_vt_sensor_read_value, vt_sensor_set),
        cmocka_unit_test_setup(test_vt_sensor_read_fingerprint, vt_sensor_set),
        cmocka_unit_test(test_vt_sensor_read_status),
        cmocka_unit_test(test_vt_sensor_calibrate),
    };

    return cmocka_run_group_tests_name("vt_core_sensor", tests, NULL, NULL);
}
