/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_raw_signature_read.h"
#include <math.h>

#define RAW_SIGNATURE_BUFFER_NOT_FILLED false
#define RAW_SIGNATURE_BUFFER_FILLED     true
#define VOLT_TO_MILLIVOLT               1000.0f

static VT_CURRENTSENSE_OBJECT* cs_object_reference;

static VT_BOOL cs_downsample_half_adc_buffer(VT_UINT raw_signatrue_buffer_index, VT_UINT adc_read_buffer_start_index)
{
    VT_UINT samples_stored =
        cs_object_reference->raw_signatures_reader->raw_signatures_shared_buffer[raw_signatrue_buffer_index].num_datapoints;
    if (samples_stored ==
        cs_object_reference->raw_signatures_reader->raw_signatures_shared_buffer[raw_signatrue_buffer_index].sample_length)
    {
        return RAW_SIGNATURE_BUFFER_FILLED;
    }

    VT_UINT downsample_factor = round(
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency /
        cs_object_reference->raw_signatures_reader->raw_signatures_shared_buffer[raw_signatrue_buffer_index].sampling_frequency);

    for (VT_UINT iter = adc_read_buffer_start_index; iter < adc_read_buffer_start_index + VT_CS_SAMPLE_LENGTH / 2;
         iter += downsample_factor)
    {
        cs_object_reference->raw_signatures_reader->raw_signatures_shared_buffer[raw_signatrue_buffer_index]
            .current_measured[samples_stored] = ((cs_object_reference->raw_signatures_reader->adc_read_buffer[iter] *
                                                     *(cs_object_reference->sensor_handle->adc_ref_volt) * VOLT_TO_MILLIVOLT) /
                                                    (VT_FLOAT)pow(2, *(cs_object_reference->sensor_handle->adc_resolution))) *
                                                (*(cs_object_reference->sensor_handle->currentsense_mV_to_mA));

        samples_stored++;
        if (samples_stored ==
            cs_object_reference->raw_signatures_reader->raw_signatures_shared_buffer[raw_signatrue_buffer_index].sample_length)
        {
            return RAW_SIGNATURE_BUFFER_FILLED;
        }
    }
    return RAW_SIGNATURE_BUFFER_NOT_FILLED;
}

static VT_BOOL cs_adc_buffer_to_raw_signature_buffers(VT_UINT adc_read_buffer_start_index)
{
    VT_BOOL all_raw_signature_buffers_filled = true;
    for (VT_UINT iter = 0; iter < cs_object_reference->raw_signatures_reader->num_raw_signatures; iter++)
    {
        all_raw_signature_buffers_filled =
            all_raw_signature_buffers_filled && cs_downsample_adc_buffer(iter, adc_read_buffer_start_index);
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

static VT_VOID cs_raw_signature_read_half_complete_callback()
{
    VT_BOOL raw_signature_buffers_filled = false;
    if (!cs_object_reference->raw_signatures_reader->raw_signature_ongoing_collection)
    {
        return;
    }

    raw_signature_buffers_filled = cs_adc_buffer_to_raw_signature_buffers(0);
    if (raw_signature_buffers_filled)
    {
        cs_object_reference->raw_signatures_reader->raw_signature_ongoing_collection = false;
    }
}

static VT_VOID cs_raw_signature_read_full_complete_callback()
{
    VT_BOOL raw_signature_buffers_filled = false;
    if (!cs_object_reference->raw_signatures_reader->raw_signature_ongoing_collection)
    {
        return;
    }
    /* Restart current acquisition */
    cs_object_reference->device_driver->adc_buffer_read(cs_object_reference->sensor_handle->adc_id,
        cs_object_reference->sensor_handle->adc_controller,
        cs_object_reference->sensor_handle->adc_channel,
        cs_object_reference->raw_signatures_reader->adc_read_buffer,
        VT_CS_SAMPLE_LENGTH,
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        &cs_raw_signature_read_half_complete_callback,
        &cs_raw_signature_read_full_complete_callback);

    raw_signature_buffers_filled = cs_adc_buffer_to_raw_signature_buffers(VT_CS_SAMPLE_LENGTH / 2);
    if (raw_signature_buffers_filled)
    {
        cs_object_reference->raw_signatures_reader->raw_signature_ongoing_collection = false;
    }
}

VT_VOID cs_raw_signature_buffer_init(
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER* raw_signature_buffer, VT_FLOAT sampling_freqeuency, VT_UINT sample_length)
{
    raw_signature_buffer->sample_length      = sample_length;
    raw_signature_buffer->num_datapoints     = 0;
    raw_signature_buffer->sampling_frequency = sampling_freqeuency;
}

VT_FLOAT cs_highest_sampling_frequency(VT_FLOAT* sampling_freqeuencies, VT_UINT num_sampling_frequencies)
{
    VT_FLOAT highest_sampling_frequency = 0;
    for (VT_UINT iter = 0; iter < num_sampling_frequencies; iter++)
    {
        if (sampling_freqeuencies[iter] > highest_sampling_frequency)
        {
            highest_sampling_frequency = sampling_freqeuencies[iter];
        }
    }
    return highest_sampling_frequency;
}

VT_UINT cs_raw_signature_read(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* sampling_frequencies, VT_UINT num_sampling_frequencies, VT_UINT sample_length)

{
    /* Update local reference */
    cs_object_reference = cs_object;

    /* Check whether the shared buffer size is sufficent and has been initialized correctly */
    if (cs_object_reference->raw_signatures_reader_initialized == false)
    {
        return VT_ERROR;
    }

    /* Set current measurement flag to true*/
    cs_object_reference->raw_signatures_reader->raw_signature_ongoing_collection = true;

    /* sample length should not be greater than the defined macro */
    if (sample_length > VT_CS_SAMPLE_LENGTH)
    {
        sample_length = VT_CS_SAMPLE_LENGTH;
    }

    /* Init number of sampling frequencies*/
    cs_object_reference->raw_signatures_reader->num_raw_signatures = num_sampling_frequencies;

    /* Initialize raw signature buffer */
    for (VT_UINT iter = 0; iter < num_sampling_frequencies; iter++)
    {
        raw_signature_buffer_init(cs_object_reference->raw_signatures_reader->raw_signatures_shared_buffer[iter],
            sampling_frequencies[iter],
            sample_length);
    }

    /* Set ADC sampling freqeuncy to max sampling frequency in the sampling_frequencies array*/
    cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency =
        cs_highest_sampling_frequency(sampling_frequencies, num_sampling_frequencies);

    /* Start current acquisition */
    cs_object_reference->device_driver->adc_buffer_read(cs_object_reference->sensor_handle->adc_id,
        cs_object_reference->sensor_handle->adc_controller,
        cs_object_reference->sensor_handle->adc_channel,
        cs_object_reference->raw_signatures_reader->adc_read_buffer,
        VT_CS_SAMPLE_LENGTH,
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        &cs_raw_signature_read_half_complete_callback,
        &cs_raw_signature_read_full_complete_callback);
}

VT_UINT cs_raw_signature_fetch_stored_current_measurement(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* raw_signature, VT_FLOAT sampling_frequency, VT_UINT sample_length)
{
    /* Check whether the shared buffer size is sufficent and has been initialized correctly */
    if (cs_object->raw_signatures_reader_initialized == false)
    {
        return VT_ERROR;
    }

    for (VT_UINT iter1 = 0; iter1 < cs_object->raw_signatures_reader->num_raw_signatures; iter1++)
    {
        if (sampling_frequency == cs_object->raw_signatures_reader->raw_signatures_shared_buffer[iter1].sampling_frequency &&
            sample_length == cs_object->raw_signatures_reader->raw_signatures_shared_buffer[iter1].sample_length)
        {
            for (VT_UINT iter2 = 0; iter2 < cs_object->raw_signatures_reader->num_raw_signatures; iter2++)
            {
                raw_signature[iter2] =
                    cs_object->raw_signatures_reader->raw_signatures_shared_buffer[iter1].current_measured[iter2];
            }
            return VT_SUCCESS;
        }
    }
    return VT_ERROR;
}
