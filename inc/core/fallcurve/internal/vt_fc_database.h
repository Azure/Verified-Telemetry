/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_FC_DATABASE_H
#define _VT_FC_DATABASE_H

#include "vt_fc_api.h"

VT_VOID fc_reset_db(VT_FALLCURVE_OBJECT* fc_object);

VT_UINT fc_store_signature(
    VT_FALLCURVE_OBJECT* fc_object, VT_ULONG sampling_interval_us, VT_ULONG falltime, VT_FLOAT pearson_coeff);

VT_UINT fc_fetch_signature(VT_FALLCURVE_OBJECT* fc_object,
    VT_UINT signature_iter,
    VT_ULONG* sampling_interval_us,
    VT_ULONG* falltime,
    VT_FLOAT* pearson_coeff);

#endif
