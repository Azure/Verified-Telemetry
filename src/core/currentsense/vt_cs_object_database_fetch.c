/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include <string.h>
#include "vt_cs_api.h"
#include "vt_cs_database.h"

VT_VOID vt_currentsense_object_database_fetch(VT_CURRENTSENSE_OBJECT* cs_object, VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db)
{
  VT_CHAR string_buffer[40];
  VT_CHAR string_element[10];
  VT_INT decimal;
  VT_FLOAT frac_float;
  VT_INT frac;

  memset(string_buffer, 0, sizeof(string_buffer));
  snprintf(string_buffer, sizeof(string_buffer), "%03d", cs_object->fingerprintdb.num_signatures);
  strcpy((VT_CHAR*)flattened_db->num_signatures, string_buffer);

  decimal = cs_object->fingerprintdb.avg_curr;
  frac_float = cs_object->fingerprintdb.avg_curr - (VT_FLOAT)decimal;
  frac = frac_float * 10000; 
  memset(string_buffer, 0, sizeof(string_buffer));
  snprintf(string_buffer, sizeof(string_buffer), "%d.%04d", decimal, frac);
  strcpy((VT_CHAR*)flattened_db->avg_curr, string_buffer);

  decimal = cs_object->fingerprintdb.lowest_sample_freq;
  frac_float = cs_object->fingerprintdb.lowest_sample_freq - (VT_FLOAT)decimal;
  frac = frac_float * 10000; 
  memset(string_buffer, 0, sizeof(string_buffer));
  snprintf(string_buffer, sizeof(string_buffer), "%d.%04d", decimal, frac);
  strcpy((VT_CHAR*)flattened_db->lowest_sample_freq, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < cs_object->fingerprintdb.num_signatures; iter++)
  {
    decimal = cs_object->fingerprintdb.db[iter].sampling_freq;
    frac_float = cs_object->fingerprintdb.db[iter].sampling_freq - (VT_FLOAT)decimal;
    frac = frac_float * 10000; 
    memset(string_element, 0, sizeof(string_element));
    snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->sampling_freq, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < cs_object->fingerprintdb.num_signatures; iter++)
  {
    decimal = cs_object->fingerprintdb.db[iter].signal_freq;
    frac_float = cs_object->fingerprintdb.db[iter].signal_freq - (VT_FLOAT)decimal;
    frac = frac_float * 10000; 
    memset(string_element, 0, sizeof(string_element));
    snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->signal_freq, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < cs_object->fingerprintdb.num_signatures; iter++)
  {
    decimal = cs_object->fingerprintdb.db[iter].curr_diff;
    frac_float = cs_object->fingerprintdb.db[iter].curr_diff - (VT_FLOAT)decimal;
    frac = frac_float * 10000; 
    memset(string_element, 0, sizeof(string_element));
    snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->curr_diff, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < cs_object->fingerprintdb.num_signatures; iter++)
  {
    decimal = cs_object->fingerprintdb.db[iter].duty_cycle;
    frac_float = cs_object->fingerprintdb.db[iter].duty_cycle - (VT_FLOAT)decimal;
    frac = frac_float * 10000; 
    memset(string_element, 0, sizeof(string_element));
    snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->duty_cycle, string_buffer);
}