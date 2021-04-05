/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"
#include "vt_test_curves.h"

#include "vt_api.h"
#include "vt_database.h"

#include <setjmp.h>
#include <stddef.h>

#include "cmocka.h"

VT_DATABASE test_database_ptr;

// Test Fixtures
static int vt_database_set(void** state)
{
    test_database_ptr._vt_total_fingerprints  = 3;
    test_database_ptr._vt_fingerprintdb[0][0] = 23;
    test_database_ptr._vt_fingerprintdb[0][1] = 100;
    memcpy(&(test_database_ptr._vt_fingerprintdb[0][2]), curve_exponential_fall, (sizeof(curve_exponential_fall)));
    test_database_ptr._vt_fingerprintdb[1][0] = 12;
    test_database_ptr._vt_fingerprintdb[1][1] = 50;
    memcpy(&(test_database_ptr._vt_fingerprintdb[1][2]), curve_triagular, (sizeof(curve_exponential_fall)));
    test_database_ptr._vt_fingerprintdb[2][0] = 23;
    test_database_ptr._vt_fingerprintdb[2][1] = 95;
    memcpy(&(test_database_ptr._vt_fingerprintdb[2][2]), curve_exponential_fall, (sizeof(curve_exponential_fall)));
    test_database_ptr._vt_total_falltime              = 3;
    test_database_ptr._vt_falltimedb[0][0]            = 12;
    test_database_ptr._vt_falltimedb[0][1]            = 1650;
    test_database_ptr._vt_falltimedb[1][0]            = 23;
    test_database_ptr._vt_falltimedb[1][1]            = 4940;
    test_database_ptr._vt_falltimedb[2][0]            = 23;
    test_database_ptr._vt_falltimedb[2][1]            = 5200;
    test_database_ptr._vt_total_pearson_coefficient   = 2;
    test_database_ptr._vt_pearson_coefficientdb[0][0] = 23;
    test_database_ptr._vt_pearson_coefficientdb[0][1] = 0.998063;
    test_database_ptr._vt_pearson_coefficientdb[1][0] = 12;
    test_database_ptr._vt_pearson_coefficientdb[1][1] = 0.991134;

    return 0;
}

static int vt_database_reset(void** state)
{
    test_database_ptr._vt_total_fingerprints = 0;
    memset(test_database_ptr._vt_fingerprintdb, 0, sizeof(test_database_ptr._vt_fingerprintdb));
    test_database_ptr._vt_total_falltime = 0;
    memset(test_database_ptr._vt_falltimedb, 0, sizeof(test_database_ptr._vt_falltimedb));
    test_database_ptr._vt_total_pearson_coefficient = 0;
    memset(test_database_ptr._vt_pearson_coefficientdb, 0, sizeof(test_database_ptr._vt_pearson_coefficientdb));

    return 0;
}

// Initialize
static void test_vt_database_initialize_fingerprintdb(void** state)
{
    (void)state;

    _vt_database_initialize_fingerprintdb(&test_database_ptr);

    assert_int_equal(test_database_ptr._vt_total_fingerprints, 0);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][1], 0);
}

static void test_vt_database_initialize_falltimedb(void** state)
{
    (void)state;

    _vt_database_initialize_falltimedb(&test_database_ptr);

    assert_int_equal(test_database_ptr._vt_total_falltime, 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 0);
}

static void test_vt_database_initialize_pearsoncoefficientdb(void** state)
{
    (void)state;

    _vt_database_initialize_pearsoncoefficientdb(&test_database_ptr);

    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 0);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 0, 0.001);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0, 0.001);
}

static void test_vt_database_initialize(void** state)
{
    (void)state;

    vt_database_initialize(&test_database_ptr, 0x00, 1);

    assert_int_equal(test_database_ptr._vt_total_fingerprints, 0);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][1], 0);
    assert_int_equal(test_database_ptr._vt_total_falltime, 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 0);
    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 0);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 0, 0.001);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0, 0.001);
}

// Store

static void test_vt_database_store_fingerprint(void** state)
{
    (void)state;

    // Static Function
}

static void test_vt_database_store_falltime(void** state)
{
    (void)state;

    assert_int_equal(test_database_ptr._vt_total_falltime, 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 0);

    assert_int_equal(_vt_database_store_falltime(&test_database_ptr, 25000, 23), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_falltime, 1);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 25000);

    assert_int_equal(_vt_database_store_falltime(&test_database_ptr, 26000, 23), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_falltime, 2);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 25000);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][1], 26000);

    assert_int_equal(_vt_database_store_falltime(&test_database_ptr, 24000, 12), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_falltime, 3);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 12);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 24000);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][1], 25000);
    assert_int_equal(test_database_ptr._vt_falltimedb[2][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[2][1], 26000);
}

static void test_vt_database_store_pearsoncoefficient(void** state)
{
    (void)state;

    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 0);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 0, 0.001);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0, 0.001);

    assert_int_equal(_vt_database_store_pearsoncoefficient(&test_database_ptr, 0.9988, 23), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 1);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 23);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0.9988, 0.001);

    assert_int_equal(_vt_database_store_pearsoncoefficient(&test_database_ptr, 0, 23), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 1);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 23);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0.9988 / 2, 0.001);

    assert_int_equal(_vt_database_store_pearsoncoefficient(&test_database_ptr, 0.8564, 12), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 2);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 23);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0.9988 / 2, 0.001);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[1][0], 12);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[1][1], 0.8564, 0.001);
}

static void test_vt_database_store_flash(void** state)
{
    (void)state;

    // Static Function
}

static void test_vt_database_store(void** state)
{
    (void)state;

    assert_int_equal(vt_database_store(&test_database_ptr, curve_exponential_fall, 100, 23), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_fingerprints, 1);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][0], 23);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][1], 100);
    for (uint32_t i = 0; i < TEST_ARRAY_LENGTH; i++)
    {
        assert_int_equal(test_database_ptr._vt_fingerprintdb[0][i + 2], curve_exponential_fall[i]);
    }
    assert_int_equal(test_database_ptr._vt_total_falltime, 1);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 5200);
    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 1);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 23);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0.998063, 0.001);

    assert_int_equal(vt_database_store(&test_database_ptr, curve_triagular, 50, 12), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_fingerprints, 2);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][0], 23);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][1], 100);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[1][0], 12);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[1][1], 50);
    for (uint32_t i = 0; i < TEST_ARRAY_LENGTH; i++)
    {
        assert_int_equal(test_database_ptr._vt_fingerprintdb[0][i + 2], curve_exponential_fall[i]);
        assert_int_equal(test_database_ptr._vt_fingerprintdb[1][i + 2], curve_triagular[i]);
    }
    assert_int_equal(test_database_ptr._vt_total_falltime, 2);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 12);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 1650);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][1], 5200);
    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 2);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 23);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0.998063, 0.001);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[1][0], 12);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[1][1], 0.991134, 0.001);

    assert_int_equal(vt_database_store(&test_database_ptr, curve_exponential_fall, 95, 23), VT_SUCCESS);

    assert_int_equal(test_database_ptr._vt_total_fingerprints, 3);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][0], 23);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][1], 100);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[1][0], 12);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[1][1], 50);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[2][0], 23);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[2][1], 95);
    for (uint32_t i = 0; i < TEST_ARRAY_LENGTH; i++)
    {
        assert_int_equal(test_database_ptr._vt_fingerprintdb[0][i + 2], curve_exponential_fall[i]);
        assert_int_equal(test_database_ptr._vt_fingerprintdb[1][i + 2], curve_triagular[i]);
        assert_int_equal(test_database_ptr._vt_fingerprintdb[2][i + 2], curve_exponential_fall[i]);
    }
    assert_int_equal(test_database_ptr._vt_total_falltime, 3);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 12);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 1650);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[1][1], 4940);
    assert_int_equal(test_database_ptr._vt_falltimedb[2][0], 23);
    assert_int_equal(test_database_ptr._vt_falltimedb[2][1], 5200);
    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 2);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 23);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0.998063, 0.001);
    assert_int_equal(test_database_ptr._vt_pearson_coefficientdb[1][0], 12);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[1][1], 0.991134, 0.001);
}

// Evaluate

static void test_vt_database_evaluate_nrmse(void** state)
{
    (void)state;

   assert_int_equal(_vt_database_evaluate_nrmse(&test_database_ptr,curve_exponential_fall),23);
   assert_int_equal(_vt_database_evaluate_nrmse(&test_database_ptr,curve_triagular),12);
}

static void test_vt_database_evaluate_pearson_falltime(void** state)
{
    (void)state;

    assert_int_equal(_vt_database_evaluate_pearson_falltime(&test_database_ptr, 5200, 0.998063), 23);
    assert_int_equal(_vt_database_evaluate_pearson_falltime(&test_database_ptr, 4940, 0.998063), 23);
    assert_int_equal(_vt_database_evaluate_pearson_falltime(&test_database_ptr, 1650, 0.991134), 12);
}

static void test_vt_database_evaluate_falltime_nearestindex_search(void** state)
{
    (void)state;

    // Static Function
}

static void test_vt_database_evaluate_pearsoncoefficient_falltimeindex_search(void** state)
{
    (void)state;

    // Static Function
}

// Fetch

static void test_vt_database_fingerprint_fetch(void** state)
{
    (void)state;

    // Unused Function

}

static void test_vt_database_falltime_fetch(void** state)
{
    (void)state;

    int index     = 0;
    int fall_time = 0;
    int sensor_id = 0;

    assert_int_equal(vt_database_falltime_fetch(&test_database_ptr, &index, &fall_time, &sensor_id), VT_SUCCESS);

    assert_int_equal(index, 1);
    assert_int_equal(fall_time, 1650);
    assert_int_equal(sensor_id, 12);
}

static void test_vt_database_pearsoncoefficient_fetch(void** state)
{
    (void)state;

    int index     = 0;
    float pearson_coefficient = 0;
    int sensor_id = 0;

    assert_int_equal(vt_database_pearsoncoefficient_fetch(&test_database_ptr, &index, &pearson_coefficient, &sensor_id), VT_SUCCESS);

    assert_int_equal(index, 1);
    assert_float_equal(pearson_coefficient, 0.998063, 0.001);
    assert_int_equal(sensor_id, 23);
}

static void test_vt_database_check_pearson_falltime_availability(void** state)
{
    (void)state;

    assert_int_equal(_vt_database_check_pearson_falltime_availability(&test_database_ptr), VT_SUCCESS);

    test_database_ptr._vt_total_falltime = 0;

    assert_int_equal(_vt_database_check_pearson_falltime_availability(&test_database_ptr), VT_ERROR);
}

// Clear

static void test_vt_database_clear(void** state)
{
    (void)state;

    vt_database_clear(&test_database_ptr);

    assert_int_equal(test_database_ptr._vt_total_fingerprints, 0);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_fingerprintdb[0][1], 0);
    assert_int_equal(test_database_ptr._vt_total_falltime, 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][0], 0);
    assert_int_equal(test_database_ptr._vt_falltimedb[0][1], 0);
    assert_int_equal(test_database_ptr._vt_total_pearson_coefficient, 0);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][0], 0, 0.001);
    assert_float_equal(test_database_ptr._vt_pearson_coefficientdb[0][1], 0, 0.001);
}

int test_vt_database()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_vt_database_initialize_fingerprintdb, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_initialize_falltimedb, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_initialize_pearsoncoefficientdb, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_initialize, vt_database_set, vt_database_reset),
        cmocka_unit_test(test_vt_database_store_fingerprint),
        cmocka_unit_test_setup_teardown(test_vt_database_store_falltime, vt_database_reset, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_store_pearsoncoefficient, vt_database_reset, vt_database_reset),
        cmocka_unit_test(test_vt_database_store_flash),
        cmocka_unit_test_setup_teardown(test_vt_database_store, vt_database_reset, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_evaluate_nrmse, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_evaluate_pearson_falltime, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_evaluate_falltime_nearestindex_search, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_evaluate_pearsoncoefficient_falltimeindex_search, vt_database_set, vt_database_reset),
        cmocka_unit_test(test_vt_database_fingerprint_fetch),
        cmocka_unit_test_setup_teardown(test_vt_database_falltime_fetch, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_pearsoncoefficient_fetch, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup_teardown(test_vt_database_check_pearson_falltime_availability, vt_database_set, vt_database_reset),
        cmocka_unit_test_setup(test_vt_database_clear, vt_database_set),
    };

    return cmocka_run_group_tests_name("vt_core_database", tests, NULL, NULL);
}
