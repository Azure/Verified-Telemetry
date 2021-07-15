/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_signature_features.h"
#include "vt_debug.h"
#include <math.h>

#define VT_CS_BUFFER_COUNT_UP             0x01
#define VT_CS_BUFFER_COUNT_DOWN           0x02
#define VT_CS_FIND_MINMAX_SEED_MAX_VALUE  -1
#define VT_CS_FIND_MINMAX_SEED_MIN_VALUE  65535
#define VT_CS_LOW_STD_DEVIATION_THRESHOLD 1.0f
#define VT_CS_PEAK_DETECTOR_SEED_POINTS   2

static VT_VOID autocorrelation(VT_FLOAT* raw_signature, VT_FLOAT* lag_array, VT_INT sample_length, VT_INT lag_length)
{
    VT_FLOAT var_sum_average = 0;
    for (VT_INT iter1 = 0; iter1 < sample_length; iter1++)
    {
        var_sum_average += raw_signature[iter1];
    }
    var_sum_average /= sample_length;
    for (VT_INT iter1 = 0; iter1 < sample_length; iter1++)
    {
        raw_signature[iter1] -= var_sum_average;
    }
    for (VT_INT iter1 = 0; iter1 < lag_length; iter1++)
    {
        lag_array[iter1] = raw_signature[iter1];
    }
    VT_INT iter2 = 0;
    VT_INT iter3 = 0;
    for (iter3 = 0; iter3 < (sample_length - lag_length); iter3++)
    {
        var_sum_average = 0;
        for (iter2 = 0; iter2 < lag_length; iter2++)
        {
            var_sum_average += (raw_signature[(iter3 + iter2)] * lag_array[iter2]);
        }
        raw_signature[iter3] = var_sum_average;
    }
    for (VT_INT iter1 = 1; iter1 < sample_length - lag_length; iter1++)
    {
        if (raw_signature[0])
        {
            raw_signature[iter1] /= raw_signature[0];
        }
    }
    raw_signature[0] = 1;
}

static VT_UINT check_acr_peak_present(VT_FLOAT* raw_signature,
    VT_UINT* index,
    VT_UINT period,
    VT_UINT* period_total,
    VT_UINT* peaks,
    VT_FLOAT minimum_correlation_for_peak)
{
    if (raw_signature[*index] > minimum_correlation_for_peak)
    {
        *peaks += 1;
        *period_total += *index;
        *index = *index + period;
        return VT_SUCCESS;
    }
    else if (raw_signature[*index + 1] > minimum_correlation_for_peak)
    {
        *peaks += 1;
        *period_total += *index;
        *index = *index + period + 1;
        return VT_SUCCESS;
    }
    else if (raw_signature[*index - 1] > minimum_correlation_for_peak)
    {
        *peaks += 1;
        *period_total += *index;
        *index = *index + period - 1;
        return VT_SUCCESS;
    }
    else
    {
        return VT_ERROR;
    }
}

static VT_UINT period_calculate(VT_FLOAT* raw_signature, VT_FLOAT* period)
{
    VT_FLOAT raw_signature_copy[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT lag_array[VT_CS_AUTO_CORRELATION_LAG]   = {0};
    VT_UINT peaks                                    = 0;
    VT_UINT index                                    = 0;
    VT_UINT period_total                             = 0;
    *period                                          = 0;

    for (VT_UINT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        raw_signature_copy[iter] = raw_signature[iter];
    }
    autocorrelation(raw_signature_copy, lag_array, VT_CS_SAMPLE_LENGTH, VT_CS_AUTO_CORRELATION_LAG);

    for (VT_UINT iter = 2; iter < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG - 2; iter++)
    {
        index = iter;
        if (((raw_signature_copy[iter] > raw_signature_copy[iter - 1]) ||
                (raw_signature_copy[iter] > raw_signature_copy[iter - 2])) &&
            ((raw_signature_copy[iter] > raw_signature_copy[iter + 1]) ||
                (raw_signature_copy[iter] > raw_signature_copy[iter + 2])))
        {
            period_total = 0;
            peaks        = 0;
            while (index < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
            {
                if (check_acr_peak_present(raw_signature_copy, &index, iter, &period_total, &peaks, VT_CS_MIN_CORRELATION) ==
                    VT_ERROR)
                {
                    break;
                }
            }
            if (index > VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
            {
                *period = iter;
                break;
            }
        }
    }
    if (peaks < 2)
    {
        *period = 0;
        return VT_ERROR;
    }
    else
    {
        *period = (((period_total * 2.0f) / (VT_FLOAT)peaks) - (VT_FLOAT)(2.0f * (*period))) / (VT_FLOAT)(peaks - 1);
        return VT_SUCCESS;
    }
}

static VT_FLOAT average_calculate(
    VT_FLOAT* raw_signature, VT_UINT sample_length, VT_UINT start_point, VT_UINT count_up_count_down)
{
    VT_FLOAT avg_curr = 0;
    if (count_up_count_down == VT_CS_BUFFER_COUNT_UP)
    {
        for (VT_UINT iter = 0; iter < sample_length; iter++)
        {
            avg_curr += raw_signature[iter];
        }
    }
    else
    {
        for (VT_UINT iter = start_point; iter > start_point - sample_length; iter--)
        {
            avg_curr += raw_signature[iter];
        }
    }

    if (sample_length)
    {
        avg_curr /= sample_length;
    }
    return avg_curr;
}

static VT_FLOAT std_dev(VT_FLOAT* raw_signature, VT_UINT sample_length, VT_UINT start_point, VT_UINT count_up_count_down)
{
    VT_FLOAT mean               = average_calculate(raw_signature, sample_length, start_point, count_up_count_down);
    VT_FLOAT standard_deviation = 0.0f;
    VT_UINT i;
    if (count_up_count_down == VT_CS_BUFFER_COUNT_UP)
    {
        for (i = 0; i < sample_length; i++)
        {
            standard_deviation += powf(raw_signature[i] - mean, 2);
        }
    }
    else
    {
        for (i = start_point; i > start_point - sample_length; i--)
        {
            standard_deviation += powf(raw_signature[i] - mean, 2);
        }
    }
    return (float)sqrt(standard_deviation / sample_length);
}

static VT_VOID min_value(VT_FLOAT* raw_signature, VT_UINT sample_length, VT_FLOAT* min_value, VT_BOOL* datapoint_visited)
{
    VT_FLOAT current_min_value     = VT_CS_FIND_MINMAX_SEED_MIN_VALUE;
    VT_UINT current_min_value_iter = 0;
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        if (datapoint_visited[iter] == true)
        {
            continue;
        }
        if (raw_signature[iter] <= current_min_value)
        {
            current_min_value      = raw_signature[iter];
            current_min_value_iter = iter;
        }
    }

    datapoint_visited[current_min_value_iter] = true;
    *min_value                                = current_min_value;
}

static VT_VOID max_value(VT_FLOAT* raw_signature, VT_UINT sample_length, VT_FLOAT* max_value, VT_BOOL* datapoint_visited)
{
    VT_FLOAT current_max_value     = VT_CS_FIND_MINMAX_SEED_MAX_VALUE;
    VT_UINT current_max_value_iter = 0;
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        if (datapoint_visited[iter] == true)
        {
            continue;
        }
        if (raw_signature[iter] > current_max_value)
        {
            current_max_value      = raw_signature[iter];
            current_max_value_iter = iter;
        }
    }

    datapoint_visited[current_max_value_iter] = true;
    *max_value                                = current_max_value;
}

static VT_VOID binary_state_current_compute(VT_FLOAT* raw_signature,
    VT_UINT sample_length,
    VT_FLOAT* curr_draw_active,
    VT_FLOAT* curr_draw_standby,
    VT_UINT* datapoints_active,
    VT_UINT* datapoints_standby)
{
    VT_FLOAT state_array[VT_CS_SAMPLE_LENGTH]   = {0};
    bool datapoint_visited[VT_CS_SAMPLE_LENGTH] = {0};
    VT_UINT low_state_count                     = 0;
    VT_UINT high_state_count                    = 0;
    VT_FLOAT z_score_low                        = 0;
    VT_FLOAT z_score_high                       = 0;
    VT_FLOAT average_low_diff_increase          = 0;
    VT_FLOAT average_high_diff_increase         = 0;
    VT_FLOAT min_max_value                      = 0;
    VT_UINT num_seedpoints =
        sample_length > (2 * VT_CS_PEAK_DETECTOR_SEED_POINTS) ? VT_CS_PEAK_DETECTOR_SEED_POINTS : (sample_length / 2);
    VT_UINT num_seedpoints_added = 0;
    VT_BOOL valid_seedpoints     = true;

    *curr_draw_active  = 0;
    *datapoints_active = 0;

    *curr_draw_standby  = 0;
    *datapoints_standby = 0;

    while (num_seedpoints_added < num_seedpoints)
    {
        min_value(raw_signature, sample_length, &state_array[low_state_count], datapoint_visited);
        max_value(raw_signature, sample_length, &state_array[sample_length - 1 - high_state_count], datapoint_visited);

        if (low_state_count && high_state_count)
        {
            if ((state_array[low_state_count] != state_array[low_state_count - 1]) &&
                (state_array[sample_length - 1 - high_state_count] != state_array[sample_length - 1 - high_state_count + 1]))
            {
                num_seedpoints_added++;
            }
        }

        if (state_array[low_state_count] == state_array[sample_length - 1 - high_state_count])
        {
            valid_seedpoints = false;
            break;
        }

        low_state_count++;
        high_state_count++;
    }

    if (valid_seedpoints == false)
    {
        low_state_count  = sample_length;
        high_state_count = 0;
        for (VT_UINT iter = 0; iter < sample_length; iter++)
        {
            state_array[iter] = raw_signature[iter];
        }
    }

    if (valid_seedpoints && (sample_length - (low_state_count + high_state_count)))
    {
        while (true)
        {
            if (low_state_count > high_state_count)
            {
                max_value(raw_signature, sample_length, &min_max_value, datapoint_visited);
            }
            else
            {
                min_value(raw_signature, sample_length, &min_max_value, datapoint_visited);
            }

            z_score_low = fabsf(min_max_value - average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP)) /
                          std_dev(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP);

            z_score_high = fabsf(min_max_value -
                                 average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN)) /
                           std_dev(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN);

            average_low_diff_increase = fabsf(
                (((average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP) * low_state_count) + min_max_value) /
                    (low_state_count + 1)) -
                average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN));

            average_high_diff_increase =
                fabsf((((average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN) *
                            high_state_count) +
                           min_max_value) /
                          (high_state_count + 1)) -
                      average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP));

            if (fabsf(z_score_high - z_score_low) < VT_CS_LOW_STD_DEVIATION_THRESHOLD)
            {
                if (average_high_diff_increase > average_low_diff_increase)
                {
                    state_array[(sample_length - 1) - high_state_count] = min_max_value;
                    high_state_count++;
                }
                else
                {
                    state_array[low_state_count] = min_max_value;
                    low_state_count++;
                }
            }
            else
            {
                if (z_score_high < z_score_low)
                {
                    state_array[(sample_length - 1) - high_state_count] = min_max_value;
                    high_state_count++;
                }
                else
                {
                    state_array[low_state_count] = min_max_value;
                    low_state_count++;
                }
            }

            if (low_state_count + high_state_count == sample_length)
            {
                break;
            }
        }
    }

    if (high_state_count)
    {
        *curr_draw_active = average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN);
    }
    *datapoints_active = high_state_count;

    if (low_state_count)
    {
        *curr_draw_standby = average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP);
    }
    *datapoints_standby = low_state_count;
}

VT_UINT cs_repeating_signature_feature_vector_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* relative_current_draw)
{
    VT_FLOAT signature_period_datapoints = 1;
    VT_FLOAT curr_draw_active            = 0;
    VT_FLOAT curr_draw_standby           = 0;
    VT_UINT datapoints_active            = 1;
    VT_UINT datapoints_standby           = 1;

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    if (period_calculate(raw_signature, &signature_period_datapoints) == VT_SUCCESS)
    {
        if (signature_period_datapoints)
        {
            *signature_frequency = sampling_frequency / signature_period_datapoints;
        }

        binary_state_current_compute(
            raw_signature, raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

        if (datapoints_standby || datapoints_active)
        {
            *duty_cycle = (VT_FLOAT)datapoints_active / (VT_FLOAT)(datapoints_standby + datapoints_active);
        }
        *relative_current_draw = curr_draw_active - curr_draw_standby;

#if VT_LOG_LEVEL > 2
        decimal    = *signature_frequency;
        frac_float = *signature_frequency - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebug("Repeating Signature Frequency = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

#if VT_LOG_LEVEL > 2
        decimal    = *relative_current_draw;
        frac_float = *relative_current_draw - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebug("Repeating Signature Relative Curr Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

#if VT_LOG_LEVEL > 2
        decimal    = *duty_cycle;
        frac_float = *duty_cycle - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebug("Repeating Signature Duty Cycle = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

        return VT_SUCCESS;
    }

    VTLogDebug("Error in computing feature vectors for repeating signature\r\n");

    return VT_ERROR;
}

VT_UINT cs_repeating_signature_offset_current_compute(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* raw_signature, VT_UINT raw_signature_length, VT_FLOAT* offset_current)
{
    VT_FLOAT curr_draw_active  = 0;
    VT_FLOAT curr_draw_standby = 0;
    VT_UINT datapoints_active  = 1;
    VT_UINT datapoints_standby = 1;

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    binary_state_current_compute(
        raw_signature, raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

    *offset_current = curr_draw_standby;

#if VT_LOG_LEVEL > 2
    decimal    = curr_draw_active;
    frac_float = curr_draw_active - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("curr_draw_active = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
#if VT_LOG_LEVEL > 2
    decimal    = curr_draw_standby;
    frac_float = curr_draw_standby - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("curr_draw_standby = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("datapoints_active = %d\r\n", datapoints_active);
    VTLogDebug("datapoints_standby = %d\r\n", datapoints_standby);

#if VT_LOG_LEVEL > 2
    decimal    = *offset_current;
    frac_float = *offset_current - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Repeating Signature Offset Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
    return VT_SUCCESS;
}

VT_UINT cs_non_repeating_signature_average_current_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT* avg_curr_on,
    VT_FLOAT* avg_curr_off)
{
    VT_FLOAT curr_draw_active  = 0;
    VT_FLOAT curr_draw_standby = 0;
    VT_UINT datapoints_active  = 1;
    VT_UINT datapoints_standby = 1;

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    binary_state_current_compute(
        raw_signature, raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

    *avg_curr_on  = curr_draw_active;
    *avg_curr_off = curr_draw_standby;

#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr_on;
    frac_float = *avg_curr_on - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Non-Repeating Signature Average ON Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr_off;
    frac_float = *avg_curr_off - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Non-Repeating Signature Average OFF Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
    return VT_SUCCESS;
}