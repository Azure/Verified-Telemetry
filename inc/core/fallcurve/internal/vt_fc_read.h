/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_FC_ADC_H
#define _VT_FC_ADC_H

#include "vt_defs.h"
#include "vt_fc_api.h"

VT_VOID fc_adc_read(VT_FALLCURVE_OBJECT* fc_object, VT_UINT* raw_signature, VT_ULONG sampling_interval_us, VT_UINT sample_length);

#endif
