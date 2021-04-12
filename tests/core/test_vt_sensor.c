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

uint32_t array[VT_FINGERPRINT_LENGTH];
char str[VT_FINGERPRINT_LENGTH];

uint32_t fingerprint_array[2][VT_FINGERPRINT_LENGTH] = {3008,3055,3035,3013,2991,2971,2953,2932,2914,2894,2875,2857,2839,2819,2820,2802,2784,2767,2749,2731,2715,2699,2684,2663,2647,2633,2610,2602,2601,2585,2569,2554,2539,2523,2505,2494,2479,2465,2450,2438,2422,2408,2405,2390,2378,2365,2351,2338,2324,2270,2300,2285,2271,2257,2244,2234,2225,2218,2203,2192,2173,2168,2155,2145,2131,2121,2108,2097,2074,2079,2072,2071,2050,2038,2025,2016,2007,1992,1982,1970,1959,1951,1938,1929,1926,1917,1907,1882,1886,1879,1865,1854,1845,1835,1826,1826,1807,1796,1795,1787,
                                                        2140,2148,2093,2052,1999,1957,1911,1897,1855,1810,1769,1759,1718,1682,1647,1635,1587,1569,1537,1527,1502,1468,1436,1427,1398,1369,991,992,959,936,935,902,899,882,863,846,842,839,807,794,780,771,758,740,737,722,694,694,692,677,663,651,647,631,621,607,608,595,580,569,567,555,545,515,530,519,508,498,491,503,474,464,468,453,443,435,434,430,414,399,403,394,386,378,378,367,360,259,253,246,243,239,233,225,229,223,217,210,211,205};



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

    assert_int_equal(vt_sensor_initialize(&sensor, "test_sensor", NULL, 7, NULL, 2, NULL), VT_SUCCESS);

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

    expect_function_call(__wrap__vt_dsc_adc_read);
    will_return(__wrap__vt_dsc_adc_read, 23);

    assert_int_equal(vt_sensor_read_value(&sensor, &value), VT_PLATFORM_SUCCESS);
    assert_int_equal(value, 23);
}

static void test_vt_sensor_read_fingerprint(void** state)
{
    (void)state;

    expect_function_call(__wrap__vt_dsc_gpio_turn_off);
    expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_port, NULL);
    expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_pin, 9);

    expect_function_calls(__wrap__vt_dsc_adc_read, VT_FINGERPRINT_LENGTH);
    expect_value_count(__wrap__vt_dsc_adc_read, adc_controller, NULL, VT_FINGERPRINT_LENGTH);
    expect_value_count(__wrap__vt_dsc_adc_read, adc_channel, 3, VT_FINGERPRINT_LENGTH);
    will_return_count(__wrap__vt_dsc_adc_read, 23, VT_FINGERPRINT_LENGTH);

    expect_function_call(__wrap__vt_dsc_gpio_turn_on);
    expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_port, NULL);
    expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_pin, 9);

    assert_int_equal(vt_sensor_read_fingerprint(&sensor, array, str), VT_PLATFORM_SUCCESS);

    for (uint8_t i = 0; i < VT_FINGERPRINT_LENGTH; i++)
    {
        assert_int_equal(array[i], 23);
    }
}

static void test_vt_sensor_read_status(void** state)
{
    (void)state;
}

static void test_vt_sensor_calibrate(void** state)
{
    (void)state;

    uint8_t confidence_metric;
    int i;

    for (int j = 0; j < 2; j++)
    {
        expect_function_call(__wrap__vt_dsc_gpio_turn_on);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_port, NULL);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_pin, 9);

        expect_function_call(__wrap__vt_dsc_adc_read);
        expect_value_count(__wrap__vt_dsc_adc_read, adc_controller, NULL, VT_FINGERPRINT_LENGTH);
        expect_value_count(__wrap__vt_dsc_adc_read, adc_channel, 3, VT_FINGERPRINT_LENGTH);

        for (i = 0; i < VT_FINGERPRINT_LENGTH; i++)
        {
            will_return(__wrap__vt_dsc_adc_read, fingerprint_array[j][i]);
        }

        expect_function_call(__wrap__vt_dsc_gpio_turn_off);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_port, NULL);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_pin, 9);
    }

    vt_sensor_calibrate(&sensor, &confidence_metric);
    assert_int_equal(sensor.vt_sampling_frequency,100);
}

uint32_t __wrap__vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay)
{
    return VT_PLATFORM_SUCCESS;
}

uint32_t __wrap__vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin)
{
    check_expected(gpio_port);
    check_expected(gpio_pin);

    function_called();

    return VT_PLATFORM_SUCCESS;
}

uint32_t __wrap__vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin)
{
    check_expected(gpio_port);
    check_expected(gpio_pin);

    function_called();

    return VT_PLATFORM_SUCCESS;
}

uint32_t __wrap__vt_dsc_adc_read(
    ADC_CONTROLLER_TYPEDEF* adc_controller, ADC_CHANNEL_TYPEDEF adc_channel, uint32_t* value)
{
    check_expected(adc_controller);
    check_expected(adc_channel);

    function_called();

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
