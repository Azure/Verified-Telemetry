/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_ADC_H
#define _VT_CS_ADC_H

#include "vt_defs.h"
#include "vt_cs_api.h"

#define VT_CS_ADC_RESOLUTION 12 
#define VT_CS_ADC_REF_VOLT 5

VT_VOID cs_adc_read(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT *raw_signature, VT_FLOAT sampling_frequency, VT_UINT sample_length);

#endif
