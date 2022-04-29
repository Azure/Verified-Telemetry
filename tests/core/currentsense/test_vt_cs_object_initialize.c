/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "test_vt_cs_definitions.h"

#include "vt_cs_api.h"

#include "cmocka.h"

// vt_currentsense_object_initialize()
static VT_VOID test_vt_currentsense_object_initialize(VT_VOID** state)
{
    VT_CURRENTSENSE_OBJECT cs_object;
    VT_DEVICE_DRIVER device_driver;
    VT_SENSOR_HANDLE sensor_handle;
    static VT_CHAR scratch_buffer_1[sizeof(VT_CURRENTSENSE_RAW_SIGNATURES_READER) - 1];
    static VT_CHAR scratch_buffer_2[sizeof(VT_CURRENTSENSE_RAW_SIGNATURES_READER)];

    assert_int_equal(
        vt_currentsense_object_initialize(&cs_object, &device_driver, &sensor_handle, scratch_buffer_1, sizeof(scratch_buffer_1)),
        VT_ERROR);
    assert_int_equal(
        vt_currentsense_object_initialize(&cs_object, &device_driver, &sensor_handle, scratch_buffer_2, sizeof(scratch_buffer_2)),
        VT_SUCCESS);
    // assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_REPEATING_SIGNATURE);
    assert_int_equal(cs_object.fingerprintdb.template.repeating_signatures.num_signatures, 0);
    assert_int_equal(cs_object.raw_signatures_reader_initialized, true);
}

VT_INT test_vt_cs_object_initialize()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_currentsense_object_initialize),
    };

    return cmocka_run_group_tests_name("test_vt_cs_object_initialize", tests, NULL, NULL);
}
