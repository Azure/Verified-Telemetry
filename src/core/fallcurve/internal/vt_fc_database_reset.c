/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_fc_database.h"

VT_VOID fc_reset_db(VT_FALLCURVE_OBJECT* fc_object)
{
    fc_object->fingerprintdb.num_signatures = 0;
}
