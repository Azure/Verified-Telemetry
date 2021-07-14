/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cmocka.h>

#include "test_vt_cs_definitions.h"
#include "test_vt_fc_definitions.h"

int main()
{
    int result = 0;

    // every test function returns the number of tests failed, 0 means success (there shouldn't be
    // negative numbers
    result += test_vt_fc_object_sensor();
    result += test_vt_fc_object_database();
    result += test_vt_fc_object_initialize();
    result += test_vt_cs_object_signature();
    result += test_vt_cs_object_database();
    result += test_vt_cs_object_initialize();
    result += test_vt_cs_object_sensor();
    return result;
}