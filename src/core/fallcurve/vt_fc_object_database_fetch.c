/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include <string.h>
#include "vt_fc_api.h"
#include "vt_fc_database.h"

VT_VOID vt_fallcurve_object_database_fetch(VT_FALLCURVE_OBJECT* fc_object, VT_FALLCURVE_DATABASE_FLATTENED* flattened_db)
{
  VT_CHAR string_buffer[40];
  VT_CHAR string_element[10];
  VT_INT decimal;
  VT_FLOAT frac_float;
  VT_INT frac;

  memset(string_buffer, 0, sizeof(string_buffer));
  snprintf(string_buffer, sizeof(string_buffer), "%03d", fc_object->fingerprintdb.num_signatures);
  strcpy((VT_CHAR*)flattened_db->num_signatures, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < fc_object->fingerprintdb.num_signatures; iter++)
  {
    snprintf(string_element, sizeof(string_element), "%lu", fc_object->fingerprintdb.db[iter].sampling_interval_us);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->sampling_interval_us, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < fc_object->fingerprintdb.num_signatures; iter++)
  {
    snprintf(string_element, sizeof(string_element), "%lu", fc_object->fingerprintdb.db[iter].falltime);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->falltime, string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));
  for(VT_UINT iter = 0; iter < fc_object->fingerprintdb.num_signatures; iter++)
  {
    decimal = fc_object->fingerprintdb.db[iter].pearson_coeff;
    frac_float = fc_object->fingerprintdb.db[iter].pearson_coeff - (VT_FLOAT)decimal;
    frac = frac_float * 10000; 
    memset(string_element, 0, sizeof(string_element));
    snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
    if(iter > 0)
    {
      strcat(string_buffer, ",");
    }
    strcat(string_buffer, string_element);
  }
  strcpy((VT_CHAR*)flattened_db->pearson_coeff, string_buffer);
}