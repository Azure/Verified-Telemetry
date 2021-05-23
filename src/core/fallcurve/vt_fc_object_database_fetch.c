/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_debug.h"
#include "vt_fc_api.h"
#include "vt_fc_database.h"


VT_VOID vt_fallcurve_object_database_fetch(VT_FALLCURVE_OBJECT* fc_object, VT_FALLCURVE_DATABASE_FLATTENED* flattened_db)
{
    VT_CHAR string_buffer[40];
    VT_CHAR string_element[10];
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
    VT_INT str_manipulation_return;
    VT_INT str_buffer_space_available;

    memset(string_buffer, 0, sizeof(string_buffer));
    str_manipulation_return = snprintf(string_buffer, sizeof(string_buffer), "%03d", fc_object->fingerprintdb.num_signatures);
    if (str_manipulation_return < 0 || (VT_UINT)str_manipulation_return > sizeof(string_buffer))
    {
        VTLogError("Flattened Database Buffer Overflow! \r\n");
    }
    strncpy((VT_CHAR*)flattened_db->num_signatures, string_buffer, sizeof(flattened_db->num_signatures) - 1);

    memset(string_buffer, 0, sizeof(string_buffer));
    for (VT_UINT iter = 0; iter < fc_object->fingerprintdb.num_signatures; iter++)
    {
        str_manipulation_return =
            snprintf(string_element, sizeof(string_element), "%lu", fc_object->fingerprintdb.db[iter].sampling_interval_us);
        if (str_manipulation_return < 0 || (VT_UINT)str_manipulation_return > sizeof(string_element))
        {
            VTLogError("Flattened Database Buffer Overflow! \r\n");
        }
        if (iter > 0)
        {
            str_buffer_space_available = sizeof(string_buffer) - strlen(string_buffer);
            strncat(string_buffer, ",", str_buffer_space_available);
        }
        str_buffer_space_available = sizeof(string_buffer) - strlen(string_buffer);
        strncat(string_buffer, string_element, str_buffer_space_available);
    }
    strncpy((VT_CHAR*)flattened_db->sampling_interval_us, string_buffer, sizeof(flattened_db->sampling_interval_us) - 1);

    memset(string_buffer, 0, sizeof(string_buffer));
    for (VT_UINT iter = 0; iter < fc_object->fingerprintdb.num_signatures; iter++)
    {
        str_manipulation_return =
            snprintf(string_element, sizeof(string_element), "%lu", fc_object->fingerprintdb.db[iter].falltime);
        if (str_manipulation_return < 0 || (VT_UINT)str_manipulation_return > sizeof(string_element))
        {
            VTLogError("Flattened Database Buffer Overflow! \r\n");
        }
        if (iter > 0)
        {
            str_buffer_space_available = sizeof(string_buffer) - strlen(string_buffer);
            strncat(string_buffer, ",", str_buffer_space_available);
        }
        str_buffer_space_available = sizeof(string_buffer) - strlen(string_buffer);
        strncat(string_buffer, string_element, str_buffer_space_available);
    }
    strncpy((VT_CHAR*)flattened_db->falltime, string_buffer, sizeof(flattened_db->falltime) - 1);

    memset(string_buffer, 0, sizeof(string_buffer));
    for (VT_UINT iter = 0; iter < fc_object->fingerprintdb.num_signatures; iter++)
    {
        decimal    = fc_object->fingerprintdb.db[iter].pearson_coeff;
        frac_float = fc_object->fingerprintdb.db[iter].pearson_coeff - (VT_FLOAT)decimal;
        frac       = frac_float * 10000;
        memset(string_element, 0, sizeof(string_element));
        str_manipulation_return = snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
        if (str_manipulation_return < 0 || (VT_UINT)str_manipulation_return > sizeof(string_element))
        {
            VTLogError("Flattened Database Buffer Overflow! \r\n");
        }
        if (iter > 0)
        {
            str_buffer_space_available = sizeof(string_buffer) - strlen(string_buffer);
            strncat(string_buffer, ",", str_buffer_space_available);
        }
        str_buffer_space_available = sizeof(string_buffer) - strlen(string_buffer);
        strncat(string_buffer, string_element, str_buffer_space_available);
    }
    strncpy((VT_CHAR*)flattened_db->pearson_coeff, string_buffer, sizeof(flattened_db->pearson_coeff) - 1);
}