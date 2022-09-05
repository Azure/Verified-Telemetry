/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_database.h"
#include <string.h>

VT_VOID vt_currentsense_object_database_fetch(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_CURRENTSENSE_DATABASE_FLATTENED* flattened_db,
    VT_BOOL* db_updated,
    VT_UINT* template_confidence_metric)
{
    VT_CHAR string_buffer[40];
    VT_CHAR string_element[15];
    int32_t decimal;
    VT_FLOAT frac_float;
    int32_t frac;

    memset(string_buffer, 0, sizeof(string_buffer));
    snprintf(string_buffer, sizeof(string_buffer), "%03d", cs_object->fingerprintdb.template_type);
    strcpy((VT_CHAR*)flattened_db->template_type, string_buffer);
    
        decimal    = cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off;
        frac_float = cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off - (VT_FLOAT)decimal;
        frac       = frac_float * 10000;
        memset(string_buffer, 0, sizeof(string_buffer));
        snprintf(string_buffer, sizeof(string_buffer), "%d.%04d", decimal, frac);
        strcpy((VT_CHAR*)flattened_db->non_repeating_signature_avg_curr_off, string_buffer);

        decimal    = cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on;
        frac_float = cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on - (VT_FLOAT)decimal;
        frac       = frac_float * 10000;
        memset(string_buffer, 0, sizeof(string_buffer));
        snprintf(string_buffer, sizeof(string_buffer), "%d.%04d", decimal, frac);
        strcpy((VT_CHAR*)flattened_db->non_repeating_signature_avg_curr_on, string_buffer);
        
        memset(string_buffer, 0, sizeof(string_buffer));
        snprintf(
            string_buffer, sizeof(string_buffer), "%03d", cs_object->fingerprintdb.template.repeating_signatures.num_signatures);
        strcpy((VT_CHAR*)flattened_db->repeating_signature_num_signatures, string_buffer);

        decimal    = cs_object->fingerprintdb.template.repeating_signatures.offset_current;
        frac_float = cs_object->fingerprintdb.template.repeating_signatures.offset_current - (VT_FLOAT)decimal;
        frac       = frac_float * 10000;
        memset(string_buffer, 0, sizeof(string_buffer));
        snprintf(string_buffer, sizeof(string_buffer), "%d.%04d", decimal, frac);
        strcpy((VT_CHAR*)flattened_db->repeating_signature_offset_curr, string_buffer);

        decimal    = cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq;
        frac_float = cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq - (VT_FLOAT)decimal;
        frac       = frac_float * 10000;
        memset(string_buffer, 0, sizeof(string_buffer));
        snprintf(string_buffer, sizeof(string_buffer), "%d.%04d", decimal, frac);
        strcpy((VT_CHAR*)flattened_db->repeating_signature_lowest_sample_freq, string_buffer);
        
        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_sampling_freq, string_buffer);
        

        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_freq, string_buffer);

        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sec_signature_freq;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sec_signature_freq - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_sec_signature_freq, string_buffer);

        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_relative_curr_draw, string_buffer);


        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_cluster_1_standby;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_cluster_1_standby - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_relative_curr_cluster_1_standby, string_buffer);


        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_cluster_2_active;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_cluster_2_active - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_relative_curr_cluster_2_active, string_buffer);


        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_average;
            frac_float =
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_average - (VT_FLOAT)decimal;
            frac = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_relative_curr_average, string_buffer);


        memset(string_buffer, 0, sizeof(string_buffer));
        for (VT_UINT iter = 0; iter < 5; iter++)
        {
            decimal    = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle;
            frac_float = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle - (VT_FLOAT)decimal;
            frac       = frac_float * 10000;
            memset(string_element, 0, sizeof(string_element));
            snprintf(string_element, sizeof(string_element), "%d.%04d", decimal, frac);
            if (iter > 0)
            {
                strcat(string_buffer, ",");
            }
            strcat(string_buffer, string_element);
        }
        strcpy((VT_CHAR*)flattened_db->repeating_signature_duty_cycle, string_buffer);


    
    *template_confidence_metric = cs_object->template_confidence_metric;

    if(cs_object->Calibration_Done_Count>=MULTICALIBRATION_COUNT)
    {
        cs_object->db_updated = VT_DB_UPDATED;
    }

    if (cs_object->db_updated == VT_DB_NOT_UPDATED)
    {
        *db_updated = false;
    }
    else
    {
        *db_updated           = true;
        cs_object->db_updated = VT_DB_NOT_UPDATED;
    }
}