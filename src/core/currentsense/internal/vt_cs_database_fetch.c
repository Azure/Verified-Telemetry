/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_database.h"
#include "vt_debug.h"

VT_UINT cs_fetch_signature(VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT signature_iter, VT_FLOAT* sampling_frequency, VT_FLOAT* signature_frequency, VT_FLOAT* duty_cycle, VT_FLOAT* current_draw)
{
  VTLogDebug("Fetching DB signature\r\n");
  if(signature_iter >= cs_object->fingerprintdb.num_signatures)
  {
    VTLogDebug("Next signature cannot be fetched from DB \r\n");
    return VT_ERROR;
  }

  *sampling_frequency = cs_object->fingerprintdb.db[signature_iter].sampling_freq;
  *signature_frequency = cs_object->fingerprintdb.db[signature_iter].signal_freq;
  *duty_cycle = cs_object->fingerprintdb.db[signature_iter].duty_cycle;
  *current_draw = cs_object->fingerprintdb.db[signature_iter].curr_diff;
  VTLogDebug("Fetched next signature from DB \r\n");
  return VT_SUCCESS;
}

VT_UINT cs_fetch_average_current(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* lowest_sample_freq, VT_FLOAT* avg_curr)
{
  if(cs_object->fingerprintdb.avg_curr == VT_CS_AVG_CURR_NOT_AVAILABLE)
  {
    VTLogDebug("Average Current unavailable in DB \r\n");
    return VT_ERROR;
  }
  *lowest_sample_freq = cs_object->fingerprintdb.lowest_sample_freq;
  *avg_curr = cs_object->fingerprintdb.avg_curr;
  return VT_SUCCESS;
}