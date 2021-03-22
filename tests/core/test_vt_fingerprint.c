/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"

#include "vt_api.h"
#include "vt_fingerprint.h"

#include <setjmp.h>
#include <stdarg.h>

#include <cmocka.h>

#define TEST_ARRAY_LENGTH 100

static int curve_zeroes[TEST_ARRAY_LENGTH]               = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int curve_constant[TEST_ARRAY_LENGTH]             = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
static int curve_exponential_rise[TEST_ARRAY_LENGTH]     = {};
static int curve_exponential_fall[TEST_ARRAY_LENGTH]     = {};
static int curve_triagular[TEST_ARRAY_LENGTH]            = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};

static void test_vt_fingerprint_calculate_maximum_index(void** state)
{
    (void)state;

    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_zeroes,TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_constant, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_exponential_rise, TEST_ARRAY_LENGTH), 100);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_exponential_fall, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_maximum_index(curve_triagular, TEST_ARRAY_LENGTH), 50);

}

static void test_vt_fingerprint_calculate_minimum_index(void** state)
{
    (void)state;

    assert_int_equal(_vt_fingerprint_calculate_minimum_index(curve_zeroes, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_minimum_index(curve_constant, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_minimum_index(curve_exponential_rise, TEST_ARRAY_LENGTH), 0);
    assert_int_equal(_vt_fingerprint_calculate_minimum_index(curve_exponential_fall, TEST_ARRAY_LENGTH), 100);
    assert_int_equal(_vt_fingerprint_calculate_minimum_index(curve_triagular, TEST_ARRAY_LENGTH), 100);

}

static void test_vt_fingerprint_calculate_calculate_37index(void** state)
{
    (void)state;
}

static void test_vt_fingerprint_calculate_falltime_pearsoncoefficient(void** state)
{
    (void)state;
}

static void test_vt_fingerprint_calculate_calculate_rankify(void** state)
{
    (void)state;
}

static void test_vt_fingerprint_calculate_shape(void** state)
{
    (void)state;
}

static void test_vt_fingerprint_evaluate_correlation_coefficient(void** state)
{
    (void)state;
}

static void test_vt_fingerprint_evaluate_nrmse(void** state)
{
    (void)state;
}

int test_vt_fingerprint()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_fingerprint_calculate_maximum_index),
        cmocka_unit_test(test_vt_fingerprint_calculate_minimum_index),
        cmocka_unit_test(test_vt_fingerprint_calculate_calculate_37index),
        cmocka_unit_test(test_vt_fingerprint_calculate_falltime_pearsoncoefficient),
        cmocka_unit_test(test_vt_fingerprint_calculate_calculate_rankify),
        cmocka_unit_test(test_vt_fingerprint_calculate_shape),
        cmocka_unit_test(test_vt_fingerprint_evaluate_correlation_coefficient),
        cmocka_unit_test(test_vt_fingerprint_evaluate_nrmse),
    };

    return cmocka_run_group_tests_name("vt_core_fingerprint", tests, NULL, NULL);
}
