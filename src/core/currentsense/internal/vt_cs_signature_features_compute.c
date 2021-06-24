/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_raw_signature_read.h"
#include "vt_cs_signature_features.h"
#include "vt_debug.h"
#include <math.h>

static VT_VOID autocorrelation(VT_FLOAT* raw_signature, VT_FLOAT* lag_array)
{
    VT_FLOAT var_sum_average = 0;
    for (VT_INT iter1 = 0; iter1 < VT_CS_SAMPLE_LENGTH; iter1++)
    {
        var_sum_average += raw_signature[iter1];
    }
    var_sum_average /= VT_CS_SAMPLE_LENGTH;
    for (VT_INT iter1 = 0; iter1 < VT_CS_SAMPLE_LENGTH; iter1++)
    {
        raw_signature[iter1] -= var_sum_average;
    }
    for (VT_INT iter1 = 0; iter1 < VT_CS_AUTO_CORRELATION_LAG * 2; iter1++)
    {
        lag_array[iter1] = raw_signature[iter1];
    }
    VT_INT iter2 = 0;
    VT_INT iter3 = 0;
    for (iter3 = 0; iter3 < (VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG); iter3++)
    {
        var_sum_average = 0;
        for (iter2 = 0; iter2 < VT_CS_AUTO_CORRELATION_LAG; iter2++)
        {
            var_sum_average += (raw_signature[(iter3 + iter2)] * lag_array[iter2]);
            //      var_sum_average += lag_array[iter2];
            //      var_sum_average += raw_signature[(iter3+iter2)];
        }
        raw_signature[iter3] = var_sum_average;
    }
    for (VT_INT iter1 = 1; iter1 < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG; iter1++)
    {
        if (raw_signature[0])
        {
            raw_signature[iter1] /= raw_signature[0];
        }
    }
    raw_signature[0] = 1;
}

static VT_UINT8 check_acr_peak_present(
    VT_FLOAT* raw_signature, VT_UINT* index, VT_INT period, VT_FLOAT* period_total, VT_UINT* peaks)
{
    if (raw_signature[*index] > VT_CS_MIN_CORRELATION)
    {
        *peaks += 1;
        *period_total += *index;
        *index = *index + period;
        return VT_SUCCESS;
    }
    else if (raw_signature[*index + 1] > VT_CS_MIN_CORRELATION)
    {
        *peaks += 1;
        *period_total += *index + 1;
        *index = *index + period + 1;
        return VT_SUCCESS;
    }
    else if (raw_signature[*index - 1] > VT_CS_MIN_CORRELATION)
    {
        *peaks += 1;
        *period_total += *index - 1;
        *index = *index + period - 1;
        return VT_SUCCESS;
    }
    else
    {
        return VT_ERROR;
    }
}

VT_UINT cs_signature_features_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* current_draw)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH]        = {0};
    VT_FLOAT lag_array[VT_CS_AUTO_CORRELATION_LAG * 2] = {0};
#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */
    cs_raw_signature_fetch_stored_current_measurement(cs_object, raw_signature, sampling_frequency, VT_CS_SAMPLE_LENGTH);
    autocorrelation(raw_signature, lag_array);
    VT_UINT peaks         = 0;
    VT_FLOAT period_total = 0;
    VT_UINT index         = 0;
    VT_UINT period        = 0;
    for (VT_INT iter = 1; iter < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG; iter++)
    {
        index = iter;
        if ((raw_signature[iter] > raw_signature[iter - 1]) && (raw_signature[iter] > raw_signature[iter + 1]))
        {
            period_total = 0;
            peaks        = 0;
            while (index < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
            {
                if (check_acr_peak_present(raw_signature, &index, iter, &period_total, &peaks))
                {
                    break;
                }
            }
            if (index > VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
            {
                period = iter;
                break;
            }
        }
    }
    if (peaks < 2)
    {
        period_total = 0;
        return VT_ERROR;
    }
    else
    {
        period_total = (((period_total * 2) / (float)peaks) - (float)(2 * period)) / (float)(peaks - 1);
    }

    VTLogDebug("CurrentSense Raw: \r\n");
#if VT_LOG_LEVEL > 2
    for (VT_INT iter = 0; iter < period_total; iter++)
    {
        decimal    = lag_array[iter];
        frac_float = lag_array[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebugNoTag("\r\n");

    VT_FLOAT min_value = 0;
    VT_FLOAT max_value = 0;
    for (VT_INT iter = 0; iter < period_total; iter++)
    {
        if (lag_array[iter] < min_value)
        {
            min_value = lag_array[iter];
        }
        else if (lag_array[iter] > max_value)
        {
            max_value = lag_array[iter];
        }
    }
    for (VT_INT iter = 0; iter < period_total; iter++)
    {
        if (lag_array[iter] > (min_value + max_value) / 2)
        {
            raw_signature[iter] = 1;
        }
        else
        {
            raw_signature[iter] = 0;
        }
    }
    VT_UINT state        = raw_signature[0];
    VT_UINT state_change = 0;
    for (VT_INT iter = 1; iter < period_total; iter++)
    {
        if (state == raw_signature[iter])
        {
            continue;
        }
        state_change++;
        state = raw_signature[iter];
    }
    VT_UINT iter1                = 0;
    state                        = raw_signature[iter1];
    VT_UINT prev_count           = 0;
    VT_UINT curr_count           = 0;
    VT_UINT next_count           = 0;
    VT_FLOAT max_state_imbalance = -period_total;
    index                        = 0;
    peaks                        = 0;
    min_value                    = 0;
    max_value                    = 0;
    while (state_change > 2)
    {
        iter1 = 0;
        state = raw_signature[iter1];
        while (iter1 < period_total)
        {
            prev_count = 0;
            curr_count = 0;
            next_count = 0;
            while (state == raw_signature[iter1] && iter1 < period_total)
            {
                prev_count++;
                iter1++;
            }
            if (iter1 >= period_total)
            {
                break;
            }
            state = raw_signature[iter1];
            index = iter1;
            while (state == raw_signature[iter1] && iter1 < period_total)
            {
                curr_count++;
                iter1++;
            }
            if (iter1 >= period_total)
            {
                break;
            }
            state = raw_signature[iter1];
            peaks = iter1;
            while (state == raw_signature[iter1] && iter1 < period_total)
            {
                next_count++;
                iter1++;
            }
            if ((((VT_FLOAT)next_count + (VT_FLOAT)prev_count) - (VT_FLOAT)curr_count) > max_state_imbalance)
            {
                max_state_imbalance = (((VT_FLOAT)next_count + (VT_FLOAT)prev_count) - (VT_FLOAT)curr_count);
                min_value           = index;
                max_value           = peaks;
            }
            iter1 = index;
            state = raw_signature[iter1];
        }
        for (VT_UINT iter2 = min_value; iter2 < max_value; iter2++)
        {
            if (raw_signature[iter2] == 1)
            {
                raw_signature[iter2] = 0;
            }
            else
            {
                raw_signature[iter2] = 1;
            }
        }
        if (state_change > 1)
        {
            state_change -= 2;
        }
    }

#if VT_LOG_LEVEL > 2
    decimal    = period_total;
    frac_float = period_total - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Signature Time Period = %d.%04d\r\n", decimal, frac);
    VTLogDebug("Processed Signature:\r\n");
#if VT_LOG_LEVEL > 2
    for (VT_INT iter = 0; iter < period_total; iter++)
    {
        decimal    = raw_signature[iter];
        frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebugNoTag("\r\n");

    VT_FLOAT avg_current_on  = 0;
    VT_FLOAT avg_current_off = 0;
    VT_FLOAT period_on       = 0;
    VT_FLOAT period_off      = 0;
    min_value                = 0;
    for (VT_INT iter = 0; iter < period_total; iter++)
    {
        if (lag_array[iter] < min_value)
        {
            min_value = lag_array[iter];
        }
    }
    for (VT_INT iter = 0; iter < period_total; iter++)
    {
        if (raw_signature[iter] == 1)
        {
            period_on++;
            avg_current_on += lag_array[iter] + fabsf(min_value);
        }
        else
        {
            period_off++;
            avg_current_off += lag_array[iter] + fabsf(min_value);
        }
    }
    avg_current_on /= period_on;
    avg_current_off /= period_off;
    *current_draw        = avg_current_on - avg_current_off;
    *duty_cycle          = period_on / period_total;
    *signature_frequency = sampling_frequency / period_total;

#if VT_LOG_LEVEL > 2
    decimal    = *current_draw;
    frac_float = *current_draw - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Signature Curr Draw = %d.%04d\r\n", decimal, frac);

#if VT_LOG_LEVEL > 2
    decimal    = *duty_cycle;
    frac_float = *duty_cycle - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Signature Duty Cycle = %d.%04d\r\n", decimal, frac);

#if VT_LOG_LEVEL > 2
    decimal    = *signature_frequency;
    frac_float = *signature_frequency - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Signature Frequency = %d.%04d\r\n", decimal, frac);

    return VT_SUCCESS;
}

VT_VOID cs_signature_features_avg_compute(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT lowest_sample_freq, VT_FLOAT* avg_curr)
{
    VT_FLOAT adc_read_signal[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT avg_curr_compute                     = 0;
#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */
    cs_raw_signature_fetch_stored_current_measurement(cs_object, adc_read_signal, lowest_sample_freq, VT_CS_SAMPLE_LENGTH);
    for (VT_UINT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        avg_curr_compute += adc_read_signal[iter];
    }
    avg_curr_compute /= VT_CS_SAMPLE_LENGTH;
    *avg_curr = avg_curr_compute;

#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr;
    frac_float = *avg_curr - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("Signature Average Current Draw = %d.%04d\r\n", decimal, frac);
}