/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_database.h"
#include "vt_cs_raw_signature_read.h"
#include "vt_cs_sensor_status.h"
#include "vt_cs_signature_features.h"

static VT_VOID cs_sensor_status_with_non_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT sampling_frequency                 = 0;
    VT_UINT num_datapoints                      = 0;
    VT_FLOAT avg_curr_on;
    VT_FLOAT avg_curr_on_saved;
    VT_FLOAT avg_curr_off;
    VT_FLOAT avg_curr_off_saved;

    VT_FLOAT avg_curr_drift = 0;

    if (cs_fetch_template_non_repeating_signature_average_current(cs_object, &avg_curr_on_saved, &avg_curr_off_saved) ==
        VT_SUCCESS)
    {
        if (cs_non_repeating_raw_signature_fetch_stored_current_measurement(
                cs_object, raw_signature, &sampling_frequency, &num_datapoints) == VT_SUCCESS)
        {

            if (cs_non_repeating_signature_average_current_compute(
                    cs_object, raw_signature, num_datapoints, &avg_curr_on, &avg_curr_off) == VT_SUCCESS)
            {
                avg_curr_drift = cs_non_repeating_signature_average_current_evaluate(
                    avg_curr_on, avg_curr_on_saved, avg_curr_off, avg_curr_off_saved);

                if (avg_curr_drift > VT_CS_MAX_AVG_CURR_DRIFT)
                {
                    cs_object->sensor_status = VT_SIGNATURE_NOT_MATCHING;
                    cs_object->sensor_drift  = avg_curr_drift;
                }
                else
                {
                    cs_object->sensor_status = VT_SIGNATURE_MATCHING;
                    cs_object->sensor_drift  = avg_curr_drift;
                }
                return;
            }
            cs_object->sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
            cs_object->sensor_drift  = 100;
        }
        cs_object->sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
        cs_object->sensor_drift  = 100;
    }

    cs_object->sensor_status = VT_SIGNATURE_DB_EMPTY;
    cs_object->sensor_drift  = 100;
}

static VT_VOID cs_sensor_status_with_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};

    VT_FLOAT lowest_sample_freq_saved;
    VT_FLOAT offset_current;
    VT_FLOAT offset_current_saved;

    VT_FLOAT sampling_frequency_saved;
    VT_FLOAT signature_frequency;
    VT_FLOAT duty_cycle;
    VT_FLOAT relative_current_draw;
    VT_FLOAT signature_frequency_saved;
    VT_FLOAT duty_cycle_saved;
    VT_FLOAT relative_current_draw_saved;

    VT_FLOAT offset_current_drift = 0;
    VT_FLOAT feature_vector_drift = 0;

    VT_BOOL offset_current_unavailable = false;

    VT_UINT signatures_evaluated                  = 0;
    VT_BOOL signature_feature_vector_compute_fail = false;
    VT_BOOL signature_offset_current_compute_fail = false;

    if (cs_fetch_template_repeating_signature_offset_current(cs_object, &lowest_sample_freq_saved, &offset_current_saved))
    {
        offset_current_unavailable = true;
    }
    if (!offset_current_unavailable)
    {
        if (cs_repeating_raw_signature_fetch_stored_current_measurement(
                cs_object, raw_signature, lowest_sample_freq_saved, VT_CS_SAMPLE_LENGTH) == VT_SUCCESS)
        {
            if (cs_repeating_signature_offset_current_compute(cs_object, raw_signature, VT_CS_SAMPLE_LENGTH, &offset_current) ==
                VT_SUCCESS)
            {
                offset_current_drift = cs_repeating_signature_offset_current_evaluate(offset_current, offset_current_saved);
            }
            else
            {
                signature_offset_current_compute_fail = true;
            }
        }
        else
        {
            signature_offset_current_compute_fail = true;
        }
    }

    for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
    {
        if (cs_fetch_template_repeating_signature_feature_vector(cs_object,
                iter,
                &sampling_frequency_saved,
                &signature_frequency_saved,
                &duty_cycle_saved,
                &relative_current_draw_saved))
        {
            break;
        }

        if (cs_repeating_raw_signature_fetch_stored_current_measurement(
                cs_object, raw_signature, sampling_frequency_saved, VT_CS_SAMPLE_LENGTH))
        {
            signature_feature_vector_compute_fail = true;
            break;
        }

        if (cs_repeating_signature_feature_vector_compute(cs_object,
                raw_signature,
                VT_CS_SAMPLE_LENGTH,
                sampling_frequency_saved,
                &signature_frequency,
                &duty_cycle,
                &relative_current_draw))
        {
            signature_feature_vector_compute_fail = true;
            break;
        }
        feature_vector_drift += cs_repeating_signature_feature_vector_evaluate(signature_frequency,
            signature_frequency_saved,
            duty_cycle,
            duty_cycle_saved,
            relative_current_draw,
            relative_current_draw_saved);
        signatures_evaluated++;
    }
    if (signatures_evaluated)
    {
        feature_vector_drift /= signatures_evaluated;
    }

    if (signature_feature_vector_compute_fail || signature_offset_current_compute_fail)
    {
        cs_object->sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
        cs_object->sensor_drift  = 100;
    }
    else if ((signatures_evaluated == 0) && offset_current_unavailable)
    {
        cs_object->sensor_status = VT_SIGNATURE_DB_EMPTY;
        cs_object->sensor_drift  = 100;
    }
    else if ((offset_current_drift > VT_CS_MAX_AVG_CURR_DRIFT) || (feature_vector_drift > VT_CS_MAX_SIGNATURE_DRIFT))
    {
        cs_object->sensor_status = VT_SIGNATURE_NOT_MATCHING;
        if (signatures_evaluated && (!offset_current_unavailable))
        {
            cs_object->sensor_drift = (offset_current_drift + feature_vector_drift) / 2;
        }
        else if (signatures_evaluated)
        {
            cs_object->sensor_drift = feature_vector_drift;
        }
        else
        {
            cs_object->sensor_drift = offset_current_drift;
        }
    }
    else
    {
        cs_object->sensor_status = VT_SIGNATURE_MATCHING;
        if (signatures_evaluated && (!offset_current_unavailable))
        {
            cs_object->sensor_drift = (offset_current_drift + feature_vector_drift) / 2;
        }
        else if (signatures_evaluated)
        {
            cs_object->sensor_drift = feature_vector_drift;
        }
        else
        {
            cs_object->sensor_drift = offset_current_drift;
        }
    }
    return;
}

VT_VOID cs_sensor_status(VT_CURRENTSENSE_OBJECT* cs_object)
{
    if (cs_object->fingerprintdb.template_type == VT_CS_NON_REPEATING_SIGNATURE)
    {
        cs_sensor_status_with_non_repeating_signature_template(cs_object);
    }
    else
    {
        cs_sensor_status_with_repeating_signature_template(cs_object);
    }
}