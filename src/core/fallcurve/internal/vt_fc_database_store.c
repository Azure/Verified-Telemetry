/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_debug.h"
#include "vt_fc_database.h"

VT_UINT fc_store_signature(
    VT_FALLCURVE_OBJECT* fc_object, VT_ULONG sampling_interval_us, VT_ULONG falltime, VT_FLOAT pearson_coeff)
{
    VT_UINT num_signatures = fc_object->fingerprintdb.num_signatures;
    if ((num_signatures + 1) > VT_FC_MAX_SIGNATURES)
    {
        return VT_ERROR;
    }

    fc_object->fingerprintdb.db[num_signatures].sampling_interval_us = sampling_interval_us;
    fc_object->fingerprintdb.db[num_signatures].falltime             = falltime;
    fc_object->fingerprintdb.db[num_signatures].pearson_coeff        = pearson_coeff;
    fc_object->fingerprintdb.num_signatures++;
    VTLogDebug("Number of Signatures stored in DB %d\r\n", fc_object->fingerprintdb.num_signatures);
    return VT_SUCCESS;
}
