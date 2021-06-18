/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_database.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

VT_VOID vt_currentsense_object_database_sync(VT_CURRENTSENSE_OBJECT* cs_object, VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db)
{
    VT_CHAR* token;
    VT_CHAR* saveptr;
    VT_CHAR* csvString;
    VT_UINT iter;

    cs_object->fingerprintdb.num_signatures     = atof((VT_CHAR*)flattened_db->num_signatures);
    cs_object->fingerprintdb.avg_curr           = (VT_FLOAT)atof((VT_CHAR*)flattened_db->avg_curr);
    cs_object->fingerprintdb.lowest_sample_freq = (VT_FLOAT)atof((VT_CHAR*)flattened_db->lowest_sample_freq);

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->sampling_freq;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        cs_object->fingerprintdb.db[iter].sampling_freq = atof((VT_CHAR*)token);
        iter++;
    }

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->signal_freq;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        cs_object->fingerprintdb.db[iter].signal_freq = atof((VT_CHAR*)token);
        iter++;
    }

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->curr_diff;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        cs_object->fingerprintdb.db[iter].curr_diff = atof((VT_CHAR*)token);
        iter++;
    }

    iter = 0;
    for (csvString = (VT_CHAR*)flattened_db->duty_cycle;; csvString = NULL)
    {
        token = strtok_r(csvString, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        cs_object->fingerprintdb.db[iter].duty_cycle = atof((VT_CHAR*)token);
        iter++;
    }
}