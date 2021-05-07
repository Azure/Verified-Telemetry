/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_FC_H
#define _VT_FC_H

#include "vt_fc_config.h"
#include "vt_fc_api.h"

typedef struct VT_FALLCURVE_TEMPLATE_SIGNATURE_STRUCT
{
   VT_ULONG sampling_interval_us;
   VT_ULONG falltime;
   VT_FLOAT pearson_coeff;
} VT_FALLCURVE_TEMPLATE_SIGNATURE;

typedef struct VT_FALLCURVE_DATABASE_STRUCT
{
   VT_FLOAT num_signatures;
   VT_FALLCURVE_TEMPLATE_SIGNATURE db[VT_FC_MAX_SIGNATURES];
} VT_FALLCURVE_DATABASE;


#endif
