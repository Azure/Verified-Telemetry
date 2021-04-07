/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_database.h"
#include "vt_fingerprint.h"
#include "vt_dsc.h"
#include "vt_debug.h"

uint32_t vt_database_initialize(VT_DATABASE* database_ptr)
{
    _vt_database_initialize_fingerprintdb(database_ptr);
    _vt_database_initialize_falltimedb(database_ptr);
    _vt_database_initialize_pearsoncoefficientdb(database_ptr);

    return VT_SUCCESS;
}

void _vt_database_initialize_fingerprintdb(VT_DATABASE* database_ptr)
{
    database_ptr->_vt_total_fingerprints = 0;
    memset(database_ptr->_vt_fingerprintdb, 0, sizeof(database_ptr->_vt_fingerprintdb));
}

void _vt_database_initialize_falltimedb(VT_DATABASE* database_ptr)
{
    database_ptr->_vt_total_falltime = 0;
    memset(database_ptr->_vt_falltimedb, 0, sizeof(database_ptr->_vt_falltimedb));
}

void _vt_database_initialize_pearsoncoefficientdb(VT_DATABASE* database_ptr)
{
    database_ptr->_vt_total_pearson_coefficient = 0;
    memset(database_ptr->_vt_pearson_coefficientdb, 0, sizeof(database_ptr->_vt_pearson_coefficientdb));
}
