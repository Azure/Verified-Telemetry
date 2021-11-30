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

    cs_object->fingerprintdb.template_type = atof((VT_CHAR*)flattened_db->template_type);
    //printf("template_type - %d",cs_object->fingerprintdb.template_type);
    if (cs_object->fingerprintdb.template_type == VT_CS_REPEATING_SIGNATURE)
    {
        cs_object->fingerprintdb.template.repeating_signatures.num_signatures =
            atof((VT_CHAR*)flattened_db->repeating_signature_num_signatures);
        cs_object->fingerprintdb.template.repeating_signatures.offset_current =
            (VT_FLOAT)atof((VT_CHAR*)flattened_db->repeating_signature_offset_curr);
        cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq =
            (VT_FLOAT)atof((VT_CHAR*)flattened_db->repeating_signature_lowest_sample_freq);

        iter = 0;
        for (csvString = (VT_CHAR*)flattened_db->repeating_signature_sampling_freq;; csvString = NULL)
        {
            token = strtok_r(csvString, ",", &saveptr);
            if (token == NULL)
            {
                break;
            }
            cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq = atof((VT_CHAR*)token);
            iter++;
        }

        iter = 0;
        for (csvString = (VT_CHAR*)flattened_db->repeating_signature_freq;; csvString = NULL)
        {
            token = strtok_r(csvString, ",", &saveptr);
            if (token == NULL)
            {
                break;
            }
            cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq = atof((VT_CHAR*)token);
            iter++;
        }

        iter = 0;
        for (csvString = (VT_CHAR*)flattened_db->repeating_signature_relative_curr_draw;; csvString = NULL)
        {
            token = strtok_r(csvString, ",", &saveptr);
            if (token == NULL)
            {
                break;
            }
            cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw = atof((VT_CHAR*)token);
            iter++;
        }

        iter = 0;
        for (csvString = (VT_CHAR*)flattened_db->repeating_signature_duty_cycle;; csvString = NULL)
        {
            token = strtok_r(csvString, ",", &saveptr);
            if (token == NULL)
            {
                break;
            }
            cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle = atof((VT_CHAR*)token);
            iter++;
        }
    }
    else if (cs_object->fingerprintdb.template_type == VT_CS_NON_REPEATING_SIGNATURE)
    {   //printf("on off synced");
        cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off =
            atof((VT_CHAR*)flattened_db->non_repeating_signature_avg_curr_off);
        cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on =
            atof((VT_CHAR*)flattened_db->non_repeating_signature_avg_curr_on);
    }
}