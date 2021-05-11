/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_CS_H
#define _VT_CS_H

#include "vt_cs_config.h"
#include "vt_cs_api.h"

typedef struct VT_CURRENTSENSE_TEMPLATE_SIGNATURE_STRUCT
{
   VT_FLOAT sampling_freq;
   VT_FLOAT signal_freq;
   VT_FLOAT curr_diff;
   VT_FLOAT duty_cycle;
} VT_CURRENTSENSE_TEMPLATE_SIGNATURE;

typedef struct VT_CURRENTSENSE_DATABASE_STRUCT
{
   VT_UINT num_signatures;
   VT_FLOAT avg_curr;
   VT_FLOAT lowest_sample_freq;
   VT_CURRENTSENSE_TEMPLATE_SIGNATURE db[VT_CS_MAX_SIGNATURES];
} VT_CURRENTSENSE_DATABASE;

#endif
