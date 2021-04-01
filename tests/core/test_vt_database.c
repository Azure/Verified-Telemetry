/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_test_definitions.h"

#include "vt_api.h"
#include "vt_database.h"

#include <setjmp.h>
#include <stdarg.h>

#include <cmocka.h>



static void test_vt_database_initialize_fingerprintdb(void** state)
{
    (void)state;
}

static void test_vt_database_initialize_falltimedb(void** state)
{
    (void)state;
}

static void test_vt_database_initialize_pearsoncoefficientdb(void** state)
{
    (void)state;
}

static void test_vt_database_store_falltime(void** state)
{
    (void)state;
}

static void test_vt_database_store_pearsoncoefficient(void** state)
{
    (void)state;
}


static void test_vt_database_check_pearson_falltime_availability(void** state)
{
    (void)state;
}

static void test_vt_database_evaluate_nrmse(void** state)
{
    (void)state;
}

static void test_vt_database_evaluate_pearson_falltime(void** state)
{
    (void)state;
}


int test_vt_database()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_database_initialize_fingerprintdb),
        cmocka_unit_test(test_vt_database_initialize_falltimedb),
        cmocka_unit_test(test_vt_database_initialize_pearsoncoefficientdb),
        cmocka_unit_test(test_vt_database_store_falltime),
        cmocka_unit_test(test_vt_database_store_pearsoncoefficient),
        cmocka_unit_test(test_vt_database_check_pearson_falltime_availability),
        cmocka_unit_test(test_vt_database_evaluate_nrmse),
        cmocka_unit_test(test_vt_database_evaluate_pearson_falltime),
    };

    return cmocka_run_group_tests_name("vt_core_database", tests, NULL, NULL);
}