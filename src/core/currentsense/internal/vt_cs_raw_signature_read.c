/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_raw_signature_read.h"
#include <math.h>

#define RAW_SIGNATURE_BUFFER_NOT_FILLED false
#define RAW_SIGNATURE_BUFFER_FILLED     true
#define VOLT_TO_MILLIVOLT               1000.0f

static VT_CURRENTSENSE_OBJECT* cs_object_reference;

static VT_BOOL cs_downsample_half_adc_buffer(
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER* raw_signature_buffer, VT_UINT adc_read_buffer_start_index)
{
    VT_UINT samples_stored = raw_signature_buffer->num_datapoints;
    if (samples_stored == raw_signature_buffer->sample_length)
    {
        return RAW_SIGNATURE_BUFFER_FILLED;
    }

    VT_FLOAT downsample_factor =
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency / raw_signature_buffer->sampling_frequency;

    VT_UINT adc_buffer_next_datapoint_to_read_index = 0;

    for (VT_UINT iter = adc_read_buffer_start_index; iter < adc_read_buffer_start_index + VT_CS_SAMPLE_LENGTH / 2; iter++)
    {
        adc_buffer_next_datapoint_to_read_index = downsample_factor * (VT_FLOAT)samples_stored;

        if (raw_signature_buffer->num_adc_buffer_datapoints_iterated != adc_buffer_next_datapoint_to_read_index)
        {
            continue;
        }

        raw_signature_buffer->num_adc_buffer_datapoints_iterated++;

        raw_signature_buffer->current_measured[samples_stored] =
            ((cs_object_reference->raw_signatures_reader->adc_read_buffer[iter] *
                 *(cs_object_reference->sensor_handle->adc_ref_volt) * VOLT_TO_MILLIVOLT) /
                (VT_FLOAT)pow(2, *(cs_object_reference->sensor_handle->adc_resolution))) *
            (*(cs_object_reference->sensor_handle->currentsense_mV_to_mA));

        samples_stored++;
        if (samples_stored == raw_signature_buffer->sample_length)
        {
            return RAW_SIGNATURE_BUFFER_FILLED;
        }
    }
    return RAW_SIGNATURE_BUFFER_NOT_FILLED;
}

static VT_VOID cs_downsample_non_repeating_raw_signature_buffer()
{
    VT_UINT samples_stored = cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_datapoints;

    /* Don't downsample if one round of data collection is not done */
    if (samples_stored == 0)
    {
        return;
    }

    VT_FLOAT downsample_factor =
        (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_adc_buffer_datapoints_iterated /
            cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length) /
        ((cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_adc_buffer_datapoints_iterated /
             cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length) +
            1);

    VT_UINT samples_stored_after_modification       = 0;
    VT_UINT adc_buffer_next_datapoint_to_read_index = 0;

    for (VT_UINT iter = 0; iter < samples_stored; iter++)
    {
        adc_buffer_next_datapoint_to_read_index = downsample_factor * (VT_FLOAT)samples_stored_after_modification;

        if (iter != adc_buffer_next_datapoint_to_read_index)
        {
            continue;
        }

        cs_object_reference->raw_signatures_reader->non_repeating_raw_signature
            .current_measured[samples_stored_after_modification] =
            cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.current_measured[iter];
        samples_stored_after_modification++;
    }
    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_datapoints = samples_stored_after_modification;

    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency /
        ((cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_adc_buffer_datapoints_iterated /
             cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length) +
            1);
}

static VT_BOOL cs_adc_buffer_to_repeating_raw_signature_buffers(VT_UINT adc_read_buffer_start_index)
{
    VT_BOOL all_raw_signature_buffers_filled = true;
    for (VT_UINT iter = 0; iter < cs_object_reference->raw_signatures_reader->num_repeating_raw_signatures; iter++)
    {
        all_raw_signature_buffers_filled =
            all_raw_signature_buffers_filled && cs_downsample_half_adc_buffer(&cs_object_reference->raw_signatures_reader->repeating_raw_signatures[iter], adc_read_buffer_start_index);
    }
    if (all_raw_signature_buffers_filled)
    {
        return RAW_SIGNATURE_BUFFER_FILLED;
    }
    else
    {
        return RAW_SIGNATURE_BUFFER_NOT_FILLED;
    }
}

static VT_VOID cs_adc_buffer_to_non_repeating_raw_signature_buffer(VT_UINT adc_read_buffer_start_index)
{
    /* Downsample existing non-repeating raw signature buffer to accomodate more datapoints */
    cs_downsample_non_repeating_raw_signature_buffer();

    /* Store new datapoints */
    cs_downsample_half_adc_buffer(&cs_object_reference->raw_signatures_reader->non_repeating_raw_signature, adc_read_buffer_start_index);
}

static VT_VOID cs_raw_signature_read_half_complete_callback()
{
    VT_BOOL repeating_raw_signature_buffers_filled = false;

    if (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature_stop_collection)
    {
        return;
    }

    /* Transfer data from adc buffer to non repeating signature buffer */
    cs_adc_buffer_to_non_repeating_raw_signature_buffer(0);

    if (cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled)
    {
        return;
    }

    /* Transfer data from adc buffer to repeating signature buffers */
    repeating_raw_signature_buffers_filled = cs_adc_buffer_to_repeating_raw_signature_buffers(0);
    if (repeating_raw_signature_buffers_filled)
    {
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    }
}

static VT_VOID cs_raw_signature_read_full_complete_callback()
{
    VT_BOOL repeating_raw_signature_buffers_filled = false;

    if (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature_stop_collection)
    {
        return;
    }

    /* Transfer data from adc buffer to non repeating signature buffer */
    cs_adc_buffer_to_non_repeating_raw_signature_buffer(0);

    /* Restart current acquisition */
    cs_object_reference->device_driver->adc_buffer_read(cs_object_reference->sensor_handle->adc_id,
        cs_object_reference->sensor_handle->adc_controller,
        cs_object_reference->sensor_handle->adc_channel,
        cs_object_reference->raw_signatures_reader->adc_read_buffer,
        VT_CS_SAMPLE_LENGTH,
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        &cs_raw_signature_read_half_complete_callback,
        &cs_raw_signature_read_full_complete_callback);
    
    if (cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled)
    {
        return;
    }
    
    /* Transfer data from adc buffer to repeating signature buffers */
    repeating_raw_signature_buffers_filled = cs_adc_buffer_to_repeating_raw_signature_buffers(VT_CS_SAMPLE_LENGTH / 2);
    if (repeating_raw_signature_buffers_filled)
    {
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    }
}

static VT_VOID cs_raw_signature_buffer_init(
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER* raw_signature_buffer, VT_FLOAT signature_sampling_frequency, VT_UINT sample_length)
{
    raw_signature_buffer->sample_length                      = sample_length;
    raw_signature_buffer->num_datapoints                     = 0;
    raw_signature_buffer->sampling_frequency                 = signature_sampling_frequency;
    raw_signature_buffer->num_adc_buffer_datapoints_iterated = 0;
}

static VT_FLOAT cs_highest_sampling_frequency(VT_FLOAT* sampling_frequencies, VT_UINT num_sampling_frequencies)
{
    VT_FLOAT highest_sampling_frequency = 0;
    for (VT_UINT iter = 0; iter < num_sampling_frequencies; iter++)
    {
        if (sampling_frequencies[iter] > highest_sampling_frequency)
        {
            highest_sampling_frequency = sampling_frequencies[iter];
        }
    }
    return highest_sampling_frequency;
}

VT_UINT cs_raw_signature_read(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* repeating_signature_sampling_frequencies,
    VT_UINT num_repeating_signature_sampling_frequencies,
    VT_UINT sample_length)
{
    /* Update local reference */
    cs_object_reference = cs_object;

    /* Check whether raw signatures buffer has been initialized correctly */
    if (cs_object_reference->raw_signatures_reader_initialized == false)
    {
        return VT_ERROR;
    }

    /* Set repeating signature current collection flag to true*/
    cs_object_reference->raw_signatures_reader->repeating_raw_signature_ongoing_collection = true;

    /* Set flag indicating whether repeating signature buffers are filled to false*/
    cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled = false;

    /* Set flag for stopping non-repeating signature current collection to false*/
    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature_stop_collection = false;

    /* sample length should not be greater than the defined macro */
    if (sample_length > VT_CS_SAMPLE_LENGTH)
    {
        sample_length = VT_CS_SAMPLE_LENGTH;
    }

    /* Init number of repeating signature sampling frequencies*/
    cs_object_reference->raw_signatures_reader->num_repeating_raw_signatures = num_repeating_signature_sampling_frequencies;

    /* Set ADC sampling freqeuncy to max sampling frequency in the repeating signature sampling frequencies array*/
    cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency =
        cs_highest_sampling_frequency(repeating_signature_sampling_frequencies, num_repeating_signature_sampling_frequencies);

    /* Initialize buffers of repeating raw signatures array */
    for (VT_UINT iter = 0; iter < num_repeating_signature_sampling_frequencies; iter++)
    {
        cs_raw_signature_buffer_init(&(cs_object_reference->raw_signatures_reader->repeating_raw_signatures[iter]),
            repeating_signature_sampling_frequencies[iter],
            sample_length);
    }

    /* Initialize buffer of non-repeating raw signature*/
    cs_raw_signature_buffer_init(&(cs_object_reference->raw_signatures_reader->non_repeating_raw_signature),
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        sample_length);

    /* Start current acquisition */
    cs_object_reference->device_driver->adc_buffer_read(cs_object_reference->sensor_handle->adc_id,
        cs_object_reference->sensor_handle->adc_controller,
        cs_object_reference->sensor_handle->adc_channel,
        cs_object_reference->raw_signatures_reader->adc_read_buffer,
        VT_CS_SAMPLE_LENGTH,
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        &cs_raw_signature_read_half_complete_callback,
        &cs_raw_signature_read_full_complete_callback);

    return VT_SUCCESS;
}

VT_UINT cs_repeating_raw_signature_fetch_stored_current_measurement(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* repeating_raw_signature, VT_FLOAT sampling_frequency, VT_UINT sample_length)
{
    /* Check whether the shared buffer size is sufficent and has been initialized correctly */
    if (cs_object->raw_signatures_reader_initialized == false)
    {
        return VT_ERROR;
    }

    /* Check whether the buffers have been stored with new current data */
    if (cs_object->raw_signatures_reader->repeating_raw_signature_buffers_filled == false)
    {
        return VT_ERROR;
    }

    /* Set to false as we are consuming the stored buffers */
    cs_object->raw_signatures_reader->repeating_raw_signature_buffers_filled = false;

    for (VT_UINT iter1 = 0; iter1 < cs_object->raw_signatures_reader->num_repeating_raw_signatures; iter1++)
    {
        if (sampling_frequency == cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].sampling_frequency &&
            sample_length == cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].sample_length)
        {
            for (VT_UINT iter2 = 0; iter2 < sample_length; iter2++)
            {
                repeating_raw_signature[iter2] =
                    cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].current_measured[iter2];
            }
            return VT_SUCCESS;
        }
    }
    return VT_ERROR;
}

VT_UINT cs_non_repeating_raw_signature_fetch_stored_current_measurement(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* non_repeating_raw_signature, VT_FLOAT* sampling_frequency, VT_UINT* num_datapoints)
{
    /* Set flag for stopping non-repeating signature current collection to true*/
    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature_stop_collection = false;

    /* Check whether the buffer has been stored with new current data */
    if (cs_object->raw_signatures_reader->non_repeating_raw_signature.num_datapoints == 0)
    {
        return VT_ERROR;
    }

    *sampling_frequency = cs_object->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency;

    *num_datapoints = cs_object->raw_signatures_reader->non_repeating_raw_signature.num_datapoints;

    for (VT_UINT iter = 0; iter < cs_object->raw_signatures_reader->non_repeating_raw_signature.num_datapoints; iter++)
    {
        non_repeating_raw_signature[iter] = cs_object->raw_signatures_reader->non_repeating_raw_signature.current_measured[iter];
    }
    return VT_SUCCESS;
}
