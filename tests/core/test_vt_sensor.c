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

uint32_t fingerprint_array[4][VT_FINGERPRINT_LENGTH] = {{2535,2584,2563,2547,2533,2518,2499,2481,2467,2449,2434,2418,2399,2388,2371,2359,2343,2342,2326,2313,2298,2285,2270,2255,2242,2229,2214,2201,2188,2176,2165,2161,2138,2135,2122,2109,2098,2086,2073,2057,2047,2034,2025,2012,1999,1998,1987,1977,1965,1952,1947,1931,1916,1909,1899,1888,1877,1865,1856,1853,1844,1832,1825,1812,1803,1791,1780,1771,1762,1752,1743,1732,1723,1721,1711,1702,1693,1683,1673,1665,1657,1647,1632,1628,1619,1611,1602,1599,1595,1584,1573,1566,1558,1550,1540,1532,1526,1514,1506,1499}, 
                                                        {2059,2129,2069,1416,1385,1369,1343,1314,1287,1262,1253,1228,1201,1177,1170,1146,1121,1099,1105,1068,1045,1025,1019,997,977,957,953,932,914,895,888,871,853,836,832,815,799,788,779,771,748,734,729,714,700,682,682,667,669,642,635,625,613,601,597,585,574,563,557,549,536,525,522,512,371,365,352,346,345,337,329,323,322,314,307,301,300,293,287,281,278,268,265,261,258,252,243,240,239,233,228,224,221,217,211,205,203,196,194,189}, 
                                                        {1363,1405,1388,1370,1356,1338,1324,1309,1299,1286,1271,1258,1239,1231,1215,1207,1196,1185,1175,1160,1149,1137,1125,1114,1103,1093,1083,1071,1062,1140,1046,1030,1021,1013,998,992,983,972,964,958,947,938,928,653,650,644,639,631,626,619,614,608,603,597,595,586,579,577,569,562,560,530,546,543,538,534,527,521,518,512,507,502,498,494,487,484,479,476,470,466,462,455,452,447,445,440,435,388,427,423,420,414,411,406,404,399,395,392,391,383}, 
                                                        {1469,1518,1502,1488,1465,1462,1449,1437,1424,1413,1402,1396,1387,1375,1363,1354,1342,1332,1321,1299,1300,1290,1289,1278,1267,1258,1249,1241,1229,1221,1211,1201,1195,1190,1181,1171,1162,1153,1151,1136,1127,1119,1111,1103,1100,1091,1043,1075,1067,1060,1051,1044,1024,1029,1021,1015,1011,1002,996,990,981,974,967,960,951,947,945,936,930,924,917,910,903,896,890,884,877,875,869,612,607,604,603,599,594,590,586,579,578,574,568,564,560,552,574,551,548,544,550,536}};



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

    uint8_t confidence_metric = 0;
    int i;

    for (int j = 0; j < 4; j++)
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

    vt_sensor_calibrate(&sensor, &confidence_metric);
    assert_int_equal(sensor.vt_sampling_frequency,6379);
    assert_int_equal(confidence_metric, 100);
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
        cmocka_unit_test_setup(test_vt_sensor_calibrate, vt_sensor_set),
    };

    return cmocka_run_group_tests_name("vt_core_sensor", tests, NULL, NULL);
}
