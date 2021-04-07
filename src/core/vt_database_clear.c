/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_api.h"

uint32_t vt_database_clear(VT_DATABASE* database_ptr)
{
    return vt_database_initialize(database_ptr);
}
