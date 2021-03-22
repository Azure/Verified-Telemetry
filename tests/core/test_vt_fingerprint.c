/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"

#include <setjmp.h>
#include <stdarg.h>

#include <cmocka.h>

static void test_vt_fingerprint_calculate_maximum_index(void** state)
{
    (void)state;
}

static void test_vt_fingerprint_calculate_minimum_index(void** state)
{
    (void)state;
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
