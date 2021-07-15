/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_calibrate.h"
#include "vt_cs_database.h"
#include "vt_cs_raw_signature_read.h"
#include "vt_cs_signature_features.h"
#include "vt_debug.h"
#include <math.h>

static VT_UINT cs_calibrate_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH]            = {0};
    VT_FLOAT top_N_frequencies[VT_CS_MAX_TEST_FREQUENCIES] = {0};
    VT_FLOAT lowest_sample_freq                            = VT_CS_ADC_MAX_SAMPLING_FREQ;
    cs_calibrate_repeating_signatures_compute_collection_settings(cs_object, top_N_frequencies, &lowest_sample_freq);

    VT_UINT8 characteristic_frequencies_found = 0;
    VT_FLOAT signal_freq;
    VT_FLOAT relative_curr_draw;
    VT_FLOAT duty_cycle;
    VT_FLOAT offset_current;

    for (VT_UINT iter = 0; iter < VT_CS_MAX_TEST_FREQUENCIES; iter++)
    {
        if (characteristic_frequencies_found == VT_CS_MAX_SIGNATURES)
        {
            break;
        }

        if (top_N_frequencies[iter] == 0)
        {
            continue;
        }

        if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
                cs_object, raw_signature, top_N_frequencies[iter], VT_CS_SAMPLE_LENGTH))
        {
            VTLogDebug("Error in fetching extrapolated raw signature! \r\n");
            continue;
        }

        if (cs_repeating_signature_feature_vector_compute(cs_object,
                raw_signature,
                VT_CS_SAMPLE_LENGTH,
                top_N_frequencies[iter],
                &signal_freq,
                &duty_cycle,
                &relative_curr_draw))
        {
            VTLogDebug("Error in computing feature vector! \r\n");
            continue;
        }

        if (characteristic_frequencies_found == 0)
        {
            cs_reset_db(cs_object);
            VTLogDebug("DB Reset! \r\n");
        }

        if (cs_store_repeating_signature_feature_vector(
                cs_object, top_N_frequencies[iter], signal_freq, duty_cycle, relative_curr_draw))
        {
            VTLogDebug("Error in storing feature vector! \r\n");
            break;
        }
        VTLogDebug("Feature vector stored successfully! \r\n");
        characteristic_frequencies_found++;
    }

    if (characteristic_frequencies_found == 0)
    {
        return VT_ERROR;
    }

    if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
            cs_object, raw_signature, lowest_sample_freq, VT_CS_SAMPLE_LENGTH))
    {
        return VT_ERROR;
    }

    if (cs_repeating_signature_offset_current_compute(cs_object, raw_signature, VT_CS_SAMPLE_LENGTH, &offset_current))
    {
        VTLogDebug("Error in computing offset current feature! \r\n");
        return VT_ERROR;
    }

    cs_update_repeating_signature_offset_current_draw(cs_object, lowest_sample_freq, offset_current);
    VTLogDebug("Offset current feature stored! \r\n");
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

static VT_UINT cs_recalibrate_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH]            = {0};
    VT_FLOAT top_N_frequencies[VT_CS_MAX_TEST_FREQUENCIES] = {0};
    VT_FLOAT lowest_sample_freq                            = VT_CS_ADC_MAX_SAMPLING_FREQ;
    cs_calibrate_repeating_signatures_compute_collection_settings(cs_object, top_N_frequencies, &lowest_sample_freq);

    VT_UINT8 characteristic_frequencies_found = 0;
    VT_FLOAT signal_freq;
    VT_FLOAT relative_curr_draw;
    VT_FLOAT duty_cycle;
    VT_FLOAT offset_current;

    for (VT_UINT iter = 0; iter < VT_CS_MAX_TEST_FREQUENCIES; iter++)
    {
        if (characteristic_frequencies_found == VT_CS_MAX_SIGNATURES)
        {
            break;
        }

        if (top_N_frequencies[iter] == 0)
        {
            continue;
        }

        if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
                cs_object, raw_signature, top_N_frequencies[iter], VT_CS_SAMPLE_LENGTH))
        {
            continue;
        }

        if (cs_repeating_signature_feature_vector_compute(cs_object,
                raw_signature,
                VT_CS_SAMPLE_LENGTH,
                top_N_frequencies[iter],
                &signal_freq,
                &duty_cycle,
                &relative_curr_draw))
        {
            VTLogDebug("Error in computing feature vector! \r\n");
            continue;
        }

        if (cs_store_repeating_signature_feature_vector(
                cs_object, top_N_frequencies[iter], signal_freq, duty_cycle, relative_curr_draw))
        {
            break;
        }
        characteristic_frequencies_found++;
    }

    if (characteristic_frequencies_found == 0)
    {
        return VT_ERROR;
    }

    if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
            cs_object, raw_signature, lowest_sample_freq, VT_CS_SAMPLE_LENGTH))
    {
        return VT_ERROR;
    }

    if (cs_repeating_signature_offset_current_compute(cs_object, raw_signature, VT_CS_SAMPLE_LENGTH, &offset_current))
    {
        return VT_ERROR;
    }

    cs_update_repeating_signature_offset_current_draw(cs_object, lowest_sample_freq, offset_current);
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

static VT_UINT cs_calibrate_non_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT sampling_frequency                 = 0;
    VT_UINT num_datapoints                      = 0;
    VT_FLOAT avg_curr_on                        = 0;
    VT_FLOAT avg_curr_off                       = 0;

    if (cs_non_repeating_raw_signature_fetch_stored_current_measurement(
            cs_object, raw_signature, &sampling_frequency, &num_datapoints) == VT_SUCCESS)
    {
        if (cs_non_repeating_signature_average_current_compute(
                cs_object, raw_signature, num_datapoints, &avg_curr_on, &avg_curr_off))
        {
            return VT_ERROR;
        }
    }

    cs_reset_db(cs_object);

    cs_update_non_repeating_signature_average_current_draw(cs_object, avg_curr_on, avg_curr_off);
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

static VT_UINT cs_recalibrate_non_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT sampling_frequency                 = 0;
    VT_UINT num_datapoints                      = 0;
    VT_FLOAT avg_curr_on                        = 0;
    VT_FLOAT avg_curr_off                       = 0;

    if (cs_non_repeating_raw_signature_fetch_stored_current_measurement(
            cs_object, raw_signature, &sampling_frequency, &num_datapoints) == VT_SUCCESS)
    {
        if (cs_non_repeating_signature_average_current_compute(
                cs_object, raw_signature, num_datapoints, &avg_curr_on, &avg_curr_off))
        {
            return VT_ERROR;
        }
    }

    cs_update_non_repeating_signature_average_current_draw(cs_object, avg_curr_on, avg_curr_off);
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

VT_VOID cs_calibrate_sensor(VT_CURRENTSENSE_OBJECT* cs_object)
{
    if (cs_calibrate_repeating_signature_template(cs_object))
    {
        if (cs_calibrate_non_repeating_signature_template(cs_object))
        {
            VTLogInfo("Error calibrating sensor\r\n");
            return;
        }
    }
    VTLogInfo("Successfully calibrated sensor\r\n");
    return;
}

VT_VOID cs_recalibrate_sensor(VT_CURRENTSENSE_OBJECT* cs_object)
{
    if (cs_recalibrate_repeating_signature_template(cs_object))
    {
        if (cs_recalibrate_non_repeating_signature_template(cs_object))
        {
            VTLogInfo("Error re-calibrating sensor\r\n");
            return;
        }
    }
    VTLogInfo("Successfully re-calibrated sensor\r\n");
    return;
}