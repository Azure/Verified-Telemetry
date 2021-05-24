/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "vt_test_definitions.h"

#include "vt_fc_api.h"
#include "vt_fc_config.h"

#include "cmocka.h"

static VT_UINT random_sampling_interval_array[VT_FC_MAX_SIGNATURES] = {26317, 6696, 278, 14072, 19332};
static VT_UINT random_falltime_array[VT_FC_MAX_SIGNATURES]          = {22914, 21767, 8921, 8518, 6110};
static VT_UINT random_pearson_coeff_array[VT_FC_MAX_SIGNATURES]     = {0.2794f, 0.9518f, 0.3319f, 0.8680f, 0.6204f};

// vt_fallcurve_object_database_fetch() & vt_fallcurve_object_database_sync()
static VT_VOID test_vt_fallcurve_object_database_fetch_sync(VT_VOID** state)
{
    VT_FALLCURVE_OBJECT fc_object;
    VT_FALLCURVE_DATABASE_FLATTENED flattened_db;
    fc_object.fingerprintdb.num_signatures = 0;

    for (VT_UINT iter = 0; iter < VT_FC_MAX_SIGNATURES; iter++)
    {
        fc_object.fingerprintdb.db[iter].sampling_interval_us = random_sampling_interval_array[iter];
        fc_object.fingerprintdb.db[iter].falltime             = random_falltime_array[iter];
        fc_object.fingerprintdb.db[iter].pearson_coeff        = random_pearson_coeff_array[iter];
        fc_object.fingerprintdb.num_signatures++;
    }

    vt_fallcurve_object_database_fetch(&fc_object, &flattened_db);

    fc_object.fingerprintdb.num_signatures = 0;
    for (VT_UINT iter = 0; iter < VT_FC_MAX_SIGNATURES; iter++)
    {
        fc_object.fingerprintdb.db[iter].sampling_interval_us = 0;
        fc_object.fingerprintdb.db[iter].falltime             = 0;
        fc_object.fingerprintdb.db[iter].pearson_coeff        = 0;
    }

    vt_fallcurve_object_database_sync(&fc_object, &flattened_db);

    assert_int_equal(fc_object.fingerprintdb.num_signatures, VT_FC_MAX_SIGNATURES);
    for (VT_UINT iter = 0; iter < VT_FC_MAX_SIGNATURES; iter++)
    {
        assert_int_equal(fc_object.fingerprintdb.db[iter].sampling_interval_us, random_sampling_interval_array[iter]);
        assert_int_equal(fc_object.fingerprintdb.db[iter].falltime, random_falltime_array[iter]);
        assert_int_equal(fc_object.fingerprintdb.db[iter].pearson_coeff, random_pearson_coeff_array[iter]);
    }
}

VT_INT test_vt_fc_object_database()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_fallcurve_object_database_fetch_sync),
    };

    return cmocka_run_group_tests_name("test_vt_fc_object_database", tests, NULL, NULL);
}
