/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_FC_SIGNATURE_H
#define _VT_FC_SIGNATURE_H

#include "vt_fc_api.h"

VT_UINT fc_signature_compute(VT_FALLCURVE_OBJECT* fc_object, VT_ULONG sampling_interval_us, VT_ULONG* falltime, VT_FLOAT* pearson_coeff);
VT_UINT fc_signature_evaluate(VT_FALLCURVE_OBJECT* fc_object, VT_ULONG falltime, VT_FLOAT pearson_coeff);
VT_UINT fc_signature_compute_collection_settings(VT_FALLCURVE_OBJECT* fc_object, VT_ULONG* sampling_interval_us, VT_UINT* confidence_metric);

#endif
