/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <stdlib.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "vt_test_definitions.h"

int main()
{
    int result = 0;

    // every test function returns the number of tests failed, 0 means success (there shouldn't be
    // negative numbers
    result += test_vt_fingerprint();
    result += test_vt_database();
    result += test_vt_sensor();

    return result;
}