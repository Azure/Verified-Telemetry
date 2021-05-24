/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "vt_fc_api.h"
#include "vt_fc_database.h"

VT_VOID vt_fallcurve_object_database_sync(VT_FALLCURVE_OBJECT* fc_object, VT_FALLCURVE_DATABASE_FLATTENED* flattened_db)
{
    VT_CHAR* token;
    VT_CHAR* saveptr;
    VT_CHAR* csvString;
    VT_UINT iter;

    fc_object->fingerprintdb.num_signatures = atof((VT_CHAR*)flattened_db->num_signatures);

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->sampling_interval_us;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        fc_object->fingerprintdb.db[iter].sampling_interval_us = atof((VT_CHAR*)token);
        iter++;
    }

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->falltime;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        fc_object->fingerprintdb.db[iter].falltime = atof((VT_CHAR*)token);
        iter++;
    }

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->pearson_coeff;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        fc_object->fingerprintdb.db[iter].pearson_coeff = atof((VT_CHAR*)token);
        iter++;
    }
}