/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "test_vt_cs_definitions.h"

#include "vt_cs_api.h"
#include "vt_cs_config.h"

#include "cmocka.h"

#define TEST_RESET_VALUE              0.0f
#define TEST_AVG_CURR_OFF_VALUE       10.45f
#define TEST_AVG_CURR_ON_VALUE        32.64f
#define TEST_LOWEST_SAMPLE_FREQ_VALUE 12.58f
#define TEST_OFFSET_CURRENT_VALUE     11.00f
#define TEST_FLOAT_EPSILON            0.1f

static VT_FLOAT test_duty_cycle_values[VT_CS_MAX_SIGNATURES]         = {0.2794f, 0.9518f, 0.3319f, 0.8680f, 0.6204f};
static VT_FLOAT test_relative_curr_draw_values[VT_CS_MAX_SIGNATURES] = {32.54f, 23.42f, 43.52f, 54.64f, 35.47f};
static VT_FLOAT test_sampling_freq_values[VT_CS_MAX_SIGNATURES]      = {5000.0f, 12.80f, 15.34f, 78.34f, 0.13f};
static VT_FLOAT test_signature_freq_values[VT_CS_MAX_SIGNATURES]     = {333.4f, 24.32f, 65.97f, 56.23f, 0.456f};

// vt_currentsense_object_database_fetch() & vt_currentsense_object_database_sync()
static VT_VOID test_vt_currentsense_object_database_fetch_sync(VT_VOID** state)
{
    VT_CURRENTSENSE_OBJECT cs_object;
    VT_CURRENTSENSE_DATABASE_FLATTENED flattened_db;
    VT_BOOL db_updated_flag;
    VT_UINT template_confidence_metric;

    cs_object.fingerprintdb.template_type                                    = VT_CS_REPEATING_SIGNATURE;
    cs_object.fingerprintdb.template.repeating_signatures.lowest_sample_freq = TEST_LOWEST_SAMPLE_FREQ_VALUE;
    cs_object.fingerprintdb.template.repeating_signatures.num_signatures     = VT_CS_MAX_SIGNATURES;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current     = TEST_OFFSET_CURRENT_VALUE;
    for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
    {
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle = test_duty_cycle_values[iter];
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw =
            test_relative_curr_draw_values[iter];
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq  = test_sampling_freq_values[iter];
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq = test_signature_freq_values[iter];
    }

    vt_currentsense_object_database_fetch(&cs_object, &flattened_db, &db_updated_flag, &template_confidence_metric);

    cs_object.fingerprintdb.template_type                                    = TEST_RESET_VALUE;
    cs_object.fingerprintdb.template.repeating_signatures.lowest_sample_freq = TEST_RESET_VALUE;
    cs_object.fingerprintdb.template.repeating_signatures.num_signatures     = TEST_RESET_VALUE;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current     = TEST_RESET_VALUE;
    for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
    {
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle         = TEST_RESET_VALUE;
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw = TEST_RESET_VALUE;
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq      = TEST_RESET_VALUE;
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq     = TEST_RESET_VALUE;
    }

    vt_currentsense_object_database_sync(&cs_object, &flattened_db);

    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_REPEATING_SIGNATURE);
    assert_float_equal(cs_object.fingerprintdb.template.repeating_signatures.lowest_sample_freq,
        TEST_LOWEST_SAMPLE_FREQ_VALUE,
        TEST_FLOAT_EPSILON);
    assert_int_equal(cs_object.fingerprintdb.template.repeating_signatures.num_signatures, VT_CS_MAX_SIGNATURES);
    assert_float_equal(
        cs_object.fingerprintdb.template.repeating_signatures.offset_current, TEST_OFFSET_CURRENT_VALUE, TEST_FLOAT_EPSILON);

    for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
    {
        assert_float_equal(cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle,
            test_duty_cycle_values[iter],
            TEST_FLOAT_EPSILON);
        assert_float_equal(cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw,
            test_relative_curr_draw_values[iter],
            TEST_FLOAT_EPSILON);
        assert_float_equal(cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq,
            test_sampling_freq_values[iter],
            TEST_FLOAT_EPSILON);
        assert_float_equal(cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq,
            test_signature_freq_values[iter],
            TEST_FLOAT_EPSILON);
    }

    cs_object.fingerprintdb.template_type                                 = VT_CS_NON_REPEATING_SIGNATURE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_off = TEST_AVG_CURR_OFF_VALUE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_on  = TEST_AVG_CURR_ON_VALUE;

    vt_currentsense_object_database_fetch(&cs_object, &flattened_db, &db_updated_flag, &template_confidence_metric);

    cs_object.fingerprintdb.template_type                                 = TEST_RESET_VALUE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_off = TEST_RESET_VALUE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_on  = TEST_RESET_VALUE;

    vt_currentsense_object_database_sync(&cs_object, &flattened_db);

    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_NON_REPEATING_SIGNATURE);
    assert_float_equal(
        cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_off, TEST_AVG_CURR_OFF_VALUE, TEST_FLOAT_EPSILON);
    assert_float_equal(
        cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_on, TEST_AVG_CURR_ON_VALUE, TEST_FLOAT_EPSILON);
}

VT_INT test_vt_cs_object_database()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_currentsense_object_database_fetch_sync),
    };

    return cmocka_run_group_tests_name("test_vt_cs_object_database", tests, NULL, NULL);
}
