/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"

#include "vt_api.h"
#include "vt_fingerprint.h"

#include <setjmp.h>
#include <stdarg.h>

#include <cmocka.h>

#define TEST_ARRAY_LENGTH 100

static uint32_t curve_zeroes[TEST_ARRAY_LENGTH]               = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint32_t curve_constant[TEST_ARRAY_LENGTH]             = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
static uint32_t curve_triagular[TEST_ARRAY_LENGTH]            = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
static uint32_t curve_exponential_fall[TEST_ARRAY_LENGTH]     = {1000,989,980,970,960,950,941,931,922,913,903,894,885,876,868,859,850,842,833,825,817,808,800,792,784,776,769,761,753,746,738,731,723,716,709,702,695,688,681,674,667,660,654,647,641,634,628,622,615,609,603,597,591,585,579,573,567,562,556,551,545,540,534,529,523,518,513,508,503,498,493,488,483,478,473,468,464,459,454,450,445,441,436,432,428,423,419,415,411,406,402,398,394,390,386,383,379,375,371,367};
static uint32_t curve_exponential_rise[TEST_ARRAY_LENGTH]     = {0,10,19,29,39,49,58,68,77,86,96,105,114,123,131,140,149,157,166,174,182,191,199,207,215,223,230,238,246,253,261,268,276,283,290,297,304,311,318,325,332,339,345,352,358,365,371,377,384,390,396,402,408,414,420,426,432,437,443,448,454,459,465,470,476,481,486,491,496,501,506,511,516,521,526,531,535,540,545,549,554,558,563,567,571,576,580,584,588,593,597,601,605,609,613,616,620,624,628,632};

static void test_vt_fingerprint_calculate_maximum_index(void** state)
{
    (void)state;

    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_zeroes, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_constant, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_triagular, TEST_ARRAY_LENGTH), 49);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_exponential_fall, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_exponential_rise, TEST_ARRAY_LENGTH), 99);

}


static void test_vt_fingerprint_calculate_37index(void** state)
{
    (void)state;

    assert_int_equal(_vt_fingerprint_calculate_37index(curve_zeroes, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_37index(curve_constant, TEST_ARRAY_LENGTH), VT_ERROR);
    assert_int_equal(_vt_fingerprint_calculate_37index(curve_triagular, TEST_ARRAY_LENGTH), 81);
    assert_int_equal(_vt_fingerprint_calculate_37index(curve_exponential_fall, TEST_ARRAY_LENGTH), 99);
    assert_int_equal(_vt_fingerprint_calculate_37index(curve_exponential_rise, TEST_ARRAY_LENGTH), 1);
}

static void test_vt_fingerprint_calculate_falltime_pearsoncoefficient(void** state)
{
    (void)state;

    int fall_time;
    float pearson_coefficient;

    assert_int_equal(_vt_fingerprint_calculate_falltime_pearsoncoefficient(curve_constant, TEST_ARRAY_LENGTH, 5, &fall_time,&pearson_coefficient), VT_ERROR);


    assert_int_equal(_vt_fingerprint_calculate_falltime_pearsoncoefficient(curve_exponential_fall, TEST_ARRAY_LENGTH, 10, &fall_time,&pearson_coefficient),VT_SUCCESS);
    assert_int_equal(fall_time,520);
    assert_float_equal(pearson_coefficient,0.998063,0.001);

    assert_int_equal(_vt_fingerprint_calculate_falltime_pearsoncoefficient(curve_triagular, TEST_ARRAY_LENGTH, 5, &fall_time,&pearson_coefficient),VT_SUCCESS);
    assert_int_equal(fall_time,165);
    assert_float_equal(pearson_coefficient,0.991134,0.001);

}

static void test_vt_fingerprint_calculate_shape(void** state)
{
    (void)state;

    assert_int_equal(_vt_fingerprint_calculate_shape(curve_zeroes, TEST_ARRAY_LENGTH), VT_SHAPE_NOISE);
    assert_int_equal(_vt_fingerprint_calculate_shape(curve_constant, TEST_ARRAY_LENGTH), VT_SHAPE_NOISE);
    assert_int_equal(_vt_fingerprint_calculate_shape(curve_triagular, TEST_ARRAY_LENGTH), VT_SHAPE_NOISE);
    assert_int_equal(_vt_fingerprint_calculate_shape(curve_exponential_fall, TEST_ARRAY_LENGTH), VT_SHAPE_FALL);
    assert_int_equal(_vt_fingerprint_calculate_shape(curve_exponential_rise, TEST_ARRAY_LENGTH), VT_SHAPE_RISE);
}

static void test_vt_fingerprint_evaluate_correlation_coefficient(void** state)
{
    (void)state;

    assert_float_equal(_vt_fingerprint_evaluate_correlationCoefficient(curve_exponential_fall, curve_exponential_rise, TEST_ARRAY_LENGTH),-1.0000,0.001);
    assert_float_equal(_vt_fingerprint_evaluate_correlationCoefficient(curve_exponential_fall, curve_exponential_fall, TEST_ARRAY_LENGTH),1.000,0.001);
    assert_float_equal(_vt_fingerprint_evaluate_correlationCoefficient(curve_triagular, curve_exponential_fall, TEST_ARRAY_LENGTH),-0.094277,0.001);
    assert_float_equal(_vt_fingerprint_evaluate_correlationCoefficient(curve_triagular, curve_exponential_rise, TEST_ARRAY_LENGTH),0.094277,0.001);

}

static void test_vt_fingerprint_evaluate_nrmse(void** state)
{
    (void)state;

    assert_float_equal(_vt_fingerprint_evaluate_nrmse(curve_constant, curve_constant, TEST_ARRAY_LENGTH), 0.00000, 0.001);
    assert_float_equal(_vt_fingerprint_evaluate_nrmse(curve_exponential_rise, curve_exponential_rise, TEST_ARRAY_LENGTH), 0.00000, 0.001);
    assert_float_equal(_vt_fingerprint_evaluate_nrmse(curve_constant, curve_exponential_rise, TEST_ARRAY_LENGTH), 81.089088, 0.001);
    assert_float_equal(_vt_fingerprint_evaluate_nrmse(curve_constant, curve_exponential_fall, TEST_ARRAY_LENGTH), 63.609119, 0.001);
    assert_float_equal(_vt_fingerprint_evaluate_nrmse(curve_constant, curve_triagular, TEST_ARRAY_LENGTH), 4.931531, 0.001);
    assert_float_equal(_vt_fingerprint_evaluate_nrmse(curve_triagular, curve_exponential_fall, TEST_ARRAY_LENGTH), 12.122904, 0.001);
}

int test_vt_fingerprint()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_fingerprint_calculate_maximum_index),
        cmocka_unit_test(test_vt_fingerprint_calculate_37index),
        cmocka_unit_test(test_vt_fingerprint_calculate_falltime_pearsoncoefficient),
        cmocka_unit_test(test_vt_fingerprint_calculate_shape),
        cmocka_unit_test(test_vt_fingerprint_evaluate_correlation_coefficient),
        cmocka_unit_test(test_vt_fingerprint_evaluate_nrmse),
    };

    return cmocka_run_group_tests_name("vt_core_fingerprint", tests, NULL, NULL);
}
