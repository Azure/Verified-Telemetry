/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_database.h"
#include "vt_debug.h"
#include <math.h>

VT_UINT cs_fetch_template_repeating_signature_feature_vector(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_UINT signature_iter,
    VT_FLOAT* sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* relative_current_draw)
{
    if (cs_object->fingerprintdb.template.repeating_signatures.num_signatures == 0)
    {
        VTLogDebug("Repeating signatures unavailable in DB\r\n");
        return VT_ERROR;
    }
    if (signature_iter >= cs_object->fingerprintdb.template.repeating_signatures.num_signatures)
    {
        VTLogDebug("Next repeating signature cannot be fetched from DB\r\n");
        return VT_ERROR;
    }

    *sampling_frequency    = cs_object->fingerprintdb.template.repeating_signatures.signatures[signature_iter].sampling_freq;
    *signature_frequency   = cs_object->fingerprintdb.template.repeating_signatures.signatures[signature_iter].signature_freq;
    *duty_cycle            = cs_object->fingerprintdb.template.repeating_signatures.signatures[signature_iter].duty_cycle;
    *relative_current_draw = cs_object->fingerprintdb.template.repeating_signatures.signatures[signature_iter].relative_curr_draw;
    return VT_SUCCESS;
}

VT_UINT cs_fetch_template_repeating_signature_offset_current(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* lowest_sample_freq, VT_FLOAT* offset_current)
{
    if (cs_object->fingerprintdb.template.repeating_signatures.offset_current == VT_DATA_NOT_AVAILABLE)
    {
        VTLogDebug("Offset Current of repeating signatures unavailable in DB\r\n");
        return VT_ERROR;
    }
    *lowest_sample_freq = cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq;
    *offset_current     = cs_object->fingerprintdb.template.repeating_signatures.offset_current;
    return VT_SUCCESS;
}

VT_UINT cs_fetch_template_non_repeating_signature_average_current(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* avg_curr_on, VT_FLOAT* avg_curr_off)
{

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    if (cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on == VT_DATA_NOT_AVAILABLE ||
        cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off == VT_DATA_NOT_AVAILABLE)
    {
        VTLogDebug("Average Current of non-repeating signature unavailable in DB \r\n");
        return VT_ERROR;
    }
    *avg_curr_on  = cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on;
    *avg_curr_off = cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off;

#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr_on;
    frac_float = *avg_curr_on - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Non-Repeating STORED Signature Average ON Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr_off;
    frac_float = *avg_curr_off - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Non-Repeating STORED Signature Average OFF Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

    return VT_SUCCESS;
}

VT_VOID cs_fetch_template_repeating_signature_sampling_frequencies(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* repeating_signature_sampling_freqeuencies,
    VT_UINT repeating_signature_sampling_frequencies_buffer_capacity,
    VT_UINT* num_repeating_signature_sampling_frequencies)
{

    #if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */
    *num_repeating_signature_sampling_frequencies = 0;
    if (repeating_signature_sampling_frequencies_buffer_capacity == 0)
    {
        return;
    }

    if (cs_object->fingerprintdb.template_type != VT_CS_REPEATING_SIGNATURE)
    {
        return;
    }

    if (cs_object->fingerprintdb.template.repeating_signatures.num_signatures == 0)
    {
        return;
    }

    VT_UINT iter=0;
    for (iter = 0; iter < cs_object->fingerprintdb.template.repeating_signatures.num_signatures; iter++)
    {
        if (iter == repeating_signature_sampling_frequencies_buffer_capacity-1)
        {
            return;
        }

        #if VT_LOG_LEVEL > 2
    decimal    = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq;
    frac_float = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("freq taken = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

        repeating_signature_sampling_freqeuencies[iter] =
            cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq;
        *num_repeating_signature_sampling_frequencies = *num_repeating_signature_sampling_frequencies + 1;
    }
    repeating_signature_sampling_freqeuencies[iter] = cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq;
    *num_repeating_signature_sampling_frequencies = *num_repeating_signature_sampling_frequencies + 1;
            #if VT_LOG_LEVEL > 2
    decimal    = cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq;
    frac_float = cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("freq taken = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
}