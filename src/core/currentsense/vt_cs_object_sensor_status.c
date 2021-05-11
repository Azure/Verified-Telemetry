/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_signature.h"
#include "vt_cs_database.h"
#include "vt_debug.h"

VT_VOID vt_currentsense_object_sensor_status(VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT* sensor_status, VT_UINT* sensor_drift)
{
  VT_FLOAT lowest_sample_freq_saved;
  VT_FLOAT avg_curr;
  VT_FLOAT avg_curr_saved;

  VT_FLOAT sampling_frequency_saved;
  VT_FLOAT signature_frequency;
  VT_FLOAT duty_cycle;
  VT_FLOAT current_draw;
  VT_FLOAT signature_frequency_saved;
  VT_FLOAT duty_cycle_saved;
  VT_FLOAT current_draw_saved;  

  VT_FLOAT avg_curr_drift = 0;
  VT_FLOAT signature_drift = 0;

  VT_BOOL avg_current_unavailable = false;

  VT_UINT signatures_evaluated = 0;
  VT_BOOL reached_db_end = false;
  VT_BOOL signature_compute_fail = false;
  VT_BOOL signature_evaluate_fail = false;

  if(cs_fetch_average_current(cs_object, &lowest_sample_freq_saved, &avg_curr_saved))
  {
    avg_current_unavailable = true;
  }
  if(!avg_current_unavailable)
  {
    cs_signature_avg_compute(cs_object, lowest_sample_freq_saved, &avg_curr);
    avg_curr_drift += cs_signature_avg_evaluate(avg_curr, avg_curr_saved);
  }

  for(VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter ++)
  {
    if(cs_fetch_signature(cs_object, iter, &sampling_frequency_saved, &signature_frequency_saved, &duty_cycle_saved, &current_draw_saved))
    {
      reached_db_end = true;
      break;
    }
    if(cs_signature_compute(cs_object, sampling_frequency_saved, &signature_frequency, &duty_cycle, &current_draw))
    {
      signature_compute_fail = true;
      break;
    }
    signature_drift += cs_signature_evaluate(signature_frequency, signature_frequency_saved, duty_cycle, duty_cycle_saved, current_draw, current_draw_saved);
    signatures_evaluated++;
  }
  if(signatures_evaluated)
  {
    signature_drift /= signatures_evaluated;
  }

  if(signature_compute_fail)
  {
    *sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
    *sensor_drift = 100;
  }
  else if (reached_db_end && (signatures_evaluated == 0) && avg_current_unavailable)
  {
    *sensor_status = VT_SIGNATURE_DB_EMPTY;
    *sensor_drift = 100;
  }
  else if((avg_curr_drift > VT_CS_MAX_AVG_CURR_DRIFT) || (signature_drift > VT_CS_MAX_SIGNATURE_DRIFT))
  {
    *sensor_status = VT_SIGNATURE_NOT_MATCHING;
    if(signatures_evaluated && (!avg_current_unavailable))
    {
      *sensor_drift = (avg_curr_drift + signature_drift)/2;
    }
    else if(signatures_evaluated)
    {
      *sensor_drift = signature_drift;
    }
    else
    {
      *sensor_drift = avg_curr_drift;
    }
  }
  else
  {
    *sensor_status = VT_SIGNATURE_MATCHING;
    if(signatures_evaluated && (!avg_current_unavailable))
    {
      *sensor_drift = (avg_curr_drift + signature_drift)/2;
    }
    else if(signatures_evaluated)
    {
      *sensor_drift = signature_drift;
    }
    else
    {
      *sensor_drift = avg_curr_drift;
    }
  }
  return;
}