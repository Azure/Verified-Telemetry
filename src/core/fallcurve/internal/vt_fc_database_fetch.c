/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_fc_database.h"

VT_UINT fc_fetch_signature(VT_FALLCURVE_OBJECT* fc_object, VT_UINT signature_iter, VT_ULONG* sampling_interval_us, VT_ULONG* falltime, VT_FLOAT* pearson_coeff)
{
  if(signature_iter >= fc_object->fingerprintdb.num_signatures)
  {
    return VT_ERROR;
  }

  *sampling_interval_us = fc_object->fingerprintdb.db[signature_iter].sampling_interval_us;
  *falltime = fc_object->fingerprintdb.db[signature_iter].falltime;
  *pearson_coeff = fc_object->fingerprintdb.db[signature_iter].pearson_coeff;
}