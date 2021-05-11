/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_database.h"

VT_VOID cs_reset_db(VT_CURRENTSENSE_OBJECT* cs_object)
{
  cs_object->fingerprintdb.num_signatures = 0;
  cs_object->fingerprintdb.lowest_sample_freq = VT_CS_ADC_MAX_SAMPLING_FREQ;
  cs_object->fingerprintdb.avg_curr = VT_CS_AVG_CURR_NOT_AVAILABLE;
}