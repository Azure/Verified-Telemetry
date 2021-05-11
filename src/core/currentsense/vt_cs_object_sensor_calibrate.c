/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include <math.h>
#include "vt_cs_api.h"
#include "vt_cs_signature.h"
#include "vt_cs_database.h"

VT_UINT vt_currentsense_sensor_calibrate(VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT8* confidence_metric)
{
    VT_FLOAT top_N_frequencies[VT_CS_MAX_TEST_FREQUENCIES] = {0};
    VT_FLOAT lowest_sample_freq = VT_CS_ADC_MAX_SAMPLING_FREQ;
    cs_signature_compute_collection_settings(cs_object, top_N_frequencies, &lowest_sample_freq);

    VT_UINT8 characteristic_frequencies_found = 0;
    VT_FLOAT sampling_freq;
    VT_FLOAT signal_freq;
    VT_FLOAT curr_diff;
    VT_FLOAT duty_cycle;
    VT_FLOAT avg_curr;
    VT_FLOAT avg_curr_repeatability_test[VT_CS_AVG_SIGNATURE_REPEATABILITY_TEST];
    VT_BOOL avg_curr_repeatability_test_result = true;
    VT_UINT8 cs_repeating_signature_confidence = 0;
    VT_UINT8 cs_avg_signature_confidence = 0;
    
    for (VT_UINT iter = 0; iter < VT_CS_MAX_TEST_FREQUENCIES; iter++)
    {
        if(characteristic_frequencies_found == VT_CS_MAX_SIGNATURES)
        {
            break;
        }
         
        if(sampling_freq == 0)
        {
            continue;
        }

        if(cs_signature_compute(cs_object, top_N_frequencies[iter], &signal_freq, &duty_cycle, &curr_diff))
        {
            continue;
        }

        if(characteristic_frequencies_found == 0)
        {
            cs_reset_db(cs_object);
        }

        if(cs_store_signature(cs_object, sampling_freq, signal_freq, duty_cycle, curr_diff))
        {
            break;
        }
        characteristic_frequencies_found++;
    }

    if(characteristic_frequencies_found)
    {
        cs_repeating_signature_confidence = 100;
    }

    cs_signature_avg_compute(cs_object, lowest_sample_freq, &avg_curr);
    for(VT_INT iter = 0; iter < VT_CS_AVG_SIGNATURE_REPEATABILITY_TEST; iter++)
    {
        cs_signature_avg_compute(cs_object, lowest_sample_freq, &(avg_curr_repeatability_test[iter]));
        if(cs_signature_avg_evaluate(avg_curr_repeatability_test[iter], avg_curr) > VT_CS_MAX_AVG_CURR_DRIFT)
        {
            avg_curr_repeatability_test_result = false;
            break;
        }
    }
    if(avg_curr_repeatability_test_result)
    {
        if(cs_update_average_current_draw(cs_object, lowest_sample_freq, avg_curr))
        {
            cs_avg_signature_confidence = 100;
        }
    }

    *confidence_metric = round(((VT_FLOAT)cs_avg_signature_confidence + (VT_FLOAT)cs_repeating_signature_confidence) / 2.0f);

    if(*confidence_metric)
    {
        return VT_SUCCESS;
    }
    return VT_ERROR;
}

VT_UINT vt_currentsense_sensor_calibrate(VT_CURRENTSENSE_OBJECT* cs_object, VT_UINT8* confidence_metric)
{
    VT_FLOAT top_N_frequencies[VT_CS_MAX_TEST_FREQUENCIES] = {0};
    VT_FLOAT lowest_sample_freq = VT_CS_ADC_MAX_SAMPLING_FREQ;
    cs_signature_compute_collection_settings(cs_object, top_N_frequencies, &lowest_sample_freq);

    VT_UINT8 characteristic_frequencies_found = 0;
    VT_FLOAT sampling_freq;
    VT_FLOAT signal_freq;
    VT_FLOAT curr_diff;
    VT_FLOAT duty_cycle;
    VT_FLOAT avg_curr;
    VT_FLOAT avg_curr_repeatability_test[VT_CS_AVG_SIGNATURE_REPEATABILITY_TEST];
    VT_BOOL avg_curr_repeatability_test_result = true;
    VT_UINT8 cs_repeating_signature_confidence = 0;
    VT_UINT8 cs_avg_signature_confidence = 0;
    
    for (VT_UINT iter = 0; iter < VT_CS_MAX_TEST_FREQUENCIES; iter++)
    {
        if(characteristic_frequencies_found == VT_CS_MAX_SIGNATURES)
        {
            break;
        }
         
        if(sampling_freq == 0)
        {
            continue;
        }

        if(cs_signature_compute(cs_object, top_N_frequencies[iter], &signal_freq, &duty_cycle, &curr_diff))
        {
            continue;
        }

        if(cs_store_signature(cs_object, sampling_freq, signal_freq, duty_cycle, curr_diff))
        {
            break;
        }
        characteristic_frequencies_found++;
    }

    if(characteristic_frequencies_found)
    {
        cs_repeating_signature_confidence = 100;
    }

    cs_signature_avg_compute(cs_object, lowest_sample_freq, &avg_curr);
    for(VT_INT iter = 0; iter < VT_CS_AVG_SIGNATURE_REPEATABILITY_TEST; iter++)
    {
        cs_signature_avg_compute(cs_object, lowest_sample_freq, &(avg_curr_repeatability_test[iter]));
        if(cs_signature_avg_evaluate(avg_curr_repeatability_test[iter], avg_curr) > VT_CS_MAX_AVG_CURR_DRIFT)
        {
            avg_curr_repeatability_test_result = false;
            break;
        }
    }
    if(avg_curr_repeatability_test_result)
    {
        if(cs_update_average_current_draw(cs_object, lowest_sample_freq, avg_curr))
        {
            cs_avg_signature_confidence = 100;
        }
    }

    *confidence_metric = round(((VT_FLOAT)cs_avg_signature_confidence + (VT_FLOAT)cs_repeating_signature_confidence) / 2.0f);

    if(*confidence_metric)
    {
        return VT_SUCCESS;
    }
    return VT_ERROR;
}
