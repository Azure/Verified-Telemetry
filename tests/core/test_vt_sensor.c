/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"
#include "vt_test_curves.h"

#include "vt_api.h"
#include "vt_dsc.h"
#include "vt_port.h"
#include "vt_sensor.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "cmocka.h"

static VT_SENSOR test_sensor;
static VT_DATABASE test_database;

static uint32_t array[VT_FINGERPRINT_LENGTH];
static char str[VT_FINGERPRINT_LENGTH];

static uint32_t fingerprint_array[2][VT_FINGERPRINT_LENGTH] = {{0,10,19,29,39,49,58,68,77,86,96,105,114,123,131,140,149,157,166,174,182,191,199,207,215,223,230,238,246,253,261,268,276,283,290,297,304,311,318,325,332,339,345,352,358,365,371,377,384,390,396,402,408,414,420,426,432,437,443,448,454,459,465,470,476,481,486,491,496,501,506,511,516,521,526,531,535,540,545,549,554,558,563,567,571,576,580,584,588,593,597,601,605,609,613,616,620,624,628,632},
                                                               {1000,989,980,970,960,950,941,931,922,913,903,894,885,876,868,859,850,842,833,825,817,808,800,792,784,776,769,761,753,746,738,731,723,716,709,702,695,688,681,674,667,660,654,647,641,634,628,622,615,609,603,597,591,585,579,573,567,562,556,551,545,540,534,529,523,518,513,508,503,498,493,488,483,478,473,468,464,459,454,450,445,441,436,432,428,423,419,415,411,406,402,398,394,390,386,383,379,375,371,367}};

// Test Fixtures
static int vt_sensor_set(void** state)
{
    test_sensor.vt_sensor_name        = strdup("set_sensor");
    test_sensor.vt_gpio_port          = NULL;
    test_sensor.vt_gpio_pin           = 9;
    test_sensor.vt_adc_controller     = NULL;
    test_sensor.vt_adc_channel        = 3;
    test_sensor.vt_timer              = NULL;
    test_sensor.vt_sampling_frequency = 10;

    return 0;
}

static int vt_database_set(void** state)
{
    test_database._vt_total_fingerprints  = 1;
    test_database._vt_fingerprintdb[0][0] = 23;
    test_database._vt_fingerprintdb[0][1] = 10;
    memcpy(&(test_database._vt_fingerprintdb[0][2]), curve_exponential_fall, (sizeof(curve_exponential_fall)));
    test_database._vt_total_falltime              = 1;
    test_database._vt_falltimedb[0][0]            = 23;
    test_database._vt_falltimedb[0][1]            = 520;
    test_database._vt_total_pearson_coefficient   = 1;
    test_database._vt_pearson_coefficientdb[0][0] = 23;
    test_database._vt_pearson_coefficientdb[0][1] = 0.998063;

    return vt_sensor_set(state);
}

// Initialize
static void test_vt_sensor_initialize(void** state)
{
    (void)state;

    assert_int_equal(vt_sensor_initialize(NULL, "test_sensor", NULL, 7, NULL, 2, NULL), VT_PTR_ERROR);

    assert_int_equal(vt_sensor_initialize(&test_sensor, "test_sensor", NULL, 7, NULL, 2, NULL), VT_SUCCESS);
    assert_string_equal(test_sensor.vt_sensor_name, "test_sensor");
    assert_ptr_equal(test_sensor.vt_gpio_port, NULL);
    assert_int_equal(test_sensor.vt_gpio_pin, 7);
    assert_ptr_equal(test_sensor.vt_adc_controller, NULL);
    assert_int_equal(test_sensor.vt_adc_channel, 2);
    assert_ptr_equal(test_sensor.vt_timer, NULL);
}

// Read
static void test_vt_sensor_read_value(void** state)
{
    (void)state;

    uint32_t value;

    assert_int_equal(vt_sensor_read_value(NULL, &value), VT_PTR_ERROR);

    expect_value(__wrap__vt_dsc_adc_read, adc_controller, NULL);
    expect_value(__wrap__vt_dsc_adc_read, adc_channel, 3);

    expect_function_call(__wrap__vt_dsc_adc_read);
    will_return(__wrap__vt_dsc_adc_read, 23);

    assert_int_equal(vt_sensor_read_value(&test_sensor, &value), VT_PLATFORM_SUCCESS);
    assert_int_equal(value, 23);
}

static void test_vt_sensor_read_fingerprint(void** state)
{
    (void)state;

    uint8_t i;

    assert_int_equal(vt_sensor_read_fingerprint(NULL, array, str), VT_PTR_ERROR);

    expect_function_call(__wrap__vt_dsc_gpio_turn_off);
    expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_port, NULL);
    expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_pin, 9);

    expect_function_calls(__wrap__vt_dsc_adc_read, VT_FINGERPRINT_LENGTH);
    expect_value_count(__wrap__vt_dsc_adc_read, adc_controller, NULL, VT_FINGERPRINT_LENGTH);
    expect_value_count(__wrap__vt_dsc_adc_read, adc_channel, 3, VT_FINGERPRINT_LENGTH);
    
    for (i = 0; i < VT_FINGERPRINT_LENGTH; i++)
    {
        will_return(__wrap__vt_dsc_adc_read, curve_exponential_fall[i]);
    }

    expect_function_call(__wrap__vt_dsc_gpio_turn_on);
    expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_port, NULL);
    expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_pin, 9);

    assert_int_equal(vt_sensor_read_fingerprint(&test_sensor, array, str), VT_PLATFORM_SUCCESS);

    for (i = 0; i < VT_FINGERPRINT_LENGTH; i++)
    {
        assert_int_equal(array[i], curve_exponential_fall[i]);
    }
}

static void test_vt_sensor_read_status(void** state)
{
    (void)state;

    int8_t sensor_id;

    assert_int_equal(vt_sensor_read_status(&test_sensor, &test_database, curve_constant, &sensor_id), VT_ERROR);

    assert_int_equal(vt_sensor_read_status(&test_sensor, &test_database, curve_exponential_fall, &sensor_id), VT_SUCCESS);
    assert_int_equal(sensor_id,23);

    test_sensor.vt_sampling_frequency = 50;
    assert_int_equal(vt_sensor_read_status(&test_sensor, &test_database, curve_triagular, &sensor_id), VT_SUCCESS);
    assert_int_equal(sensor_id,0);

    test_database._vt_total_falltime = 0;
    assert_int_equal(vt_sensor_read_status(&test_sensor, &test_database, curve_triagular, &sensor_id), VT_SUCCESS);
    assert_int_equal(sensor_id, -1);
}

// Calibrate
static void test_vt_sensor_calibrate(void** state)
{
    (void)state;

    uint8_t confidence_metric = 0;
    uint8_t i;
    uint8_t j;

    for (j = 0; j < 2; j++)
    {
        expect_function_call(__wrap__vt_dsc_gpio_turn_off);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_port, NULL);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_pin, 9);

        expect_function_calls(__wrap__vt_dsc_adc_read, VT_FINGERPRINT_LENGTH);
        expect_value_count(__wrap__vt_dsc_adc_read, adc_controller, NULL, VT_FINGERPRINT_LENGTH);
        expect_value_count(__wrap__vt_dsc_adc_read, adc_channel, 3, VT_FINGERPRINT_LENGTH);

        for (i = 0; i < VT_FINGERPRINT_LENGTH; i++)
        {
            will_return(__wrap__vt_dsc_adc_read, fingerprint_array[j][i]);
        }

        expect_function_call(__wrap__vt_dsc_gpio_turn_on);
        expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_port, NULL);
        expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_pin, 9);
    }

    vt_sensor_calibrate(&test_sensor, &confidence_metric);
    assert_int_equal(test_sensor.vt_sampling_frequency,VT_MAXIMUM_FREQUENCY);
    assert_int_equal(confidence_metric, 50);

    for (j = 0; j < 3; j++)
    {
        expect_function_call(__wrap__vt_dsc_gpio_turn_off);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_port, NULL);
        expect_value(__wrap__vt_dsc_gpio_turn_off, gpio_pin, 9);

        expect_function_calls(__wrap__vt_dsc_adc_read, VT_FINGERPRINT_LENGTH);
        expect_value_count(__wrap__vt_dsc_adc_read, adc_controller, NULL, VT_FINGERPRINT_LENGTH);
        expect_value_count(__wrap__vt_dsc_adc_read, adc_channel, 3, VT_FINGERPRINT_LENGTH);

        for (i = 0; i < VT_FINGERPRINT_LENGTH; i++)
        {
            will_return(__wrap__vt_dsc_adc_read, curve_constant[i]);
        }

        expect_function_call(__wrap__vt_dsc_gpio_turn_on);
        expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_port, NULL);
        expect_value(__wrap__vt_dsc_gpio_turn_on, gpio_pin, 9);
    }

    vt_sensor_calibrate(&test_sensor, &confidence_metric);
    assert_int_equal(test_sensor.vt_sampling_frequency, VT_MINIMUM_FREQUENCY);
    assert_int_equal(confidence_metric, 0);
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
        cmocka_unit_test_setup(test_vt_sensor_read_status, vt_database_set),
        cmocka_unit_test_setup(test_vt_sensor_calibrate, vt_sensor_set),
    };

    return cmocka_run_group_tests_name("vt_core_sensor", tests, NULL, NULL);
}
