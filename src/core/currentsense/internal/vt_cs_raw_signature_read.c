/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_raw_signature_read.h"
#include "vt_debug.h"
#include <math.h>

#define RAW_SIGNATURE_BUFFER_NOT_FILLED false
#define RAW_SIGNATURE_BUFFER_FILLED     true
#define VOLT_TO_MILLIVOLT               1000.0f

static VT_CURRENTSENSE_OBJECT* cs_object_reference;

static VT_BOOL cs_downsample_half_adc_buffer(
    VT_CURRENTSENSE_RAW_SIGNATURE_BUFFER* raw_signature_buffer, VT_UINT adc_read_buffer_start_index)
<<<<<<< HEAD
{   //printf("cs_downsample \n");
=======
{   
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
    if (raw_signature_buffer->num_datapoints == raw_signature_buffer->sample_length)
    {
        return RAW_SIGNATURE_BUFFER_FILLED;
    }
    //printf("for sampling_frequency - %d \n",(int)raw_signature_buffer->sampling_frequency);
    //printf("next datapoint %d\n",raw_signature_buffer->num_datapoints);
    VT_FLOAT downsample_factor =
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency / raw_signature_buffer->sampling_frequency;

    VT_UINT adc_buffer_next_datapoint_to_read_index = 0;

    for (VT_UINT iter = adc_read_buffer_start_index; iter < adc_read_buffer_start_index + VT_CS_SAMPLE_LENGTH / 2; iter++)
    {   //printf("inside for\n");
        adc_buffer_next_datapoint_to_read_index = downsample_factor * (VT_FLOAT)raw_signature_buffer->num_datapoints;
        //printf("next datapoint %d\n",adc_buffer_next_datapoint_to_read_index);
        if (raw_signature_buffer->num_adc_buffer_datapoints_iterated != adc_buffer_next_datapoint_to_read_index)
        {   
            raw_signature_buffer->num_adc_buffer_datapoints_iterated++;
            continue;
        }

        raw_signature_buffer->num_adc_buffer_datapoints_iterated++;

        raw_signature_buffer->current_measured[raw_signature_buffer->num_datapoints] =
            ((cs_object_reference->raw_signatures_reader->adc_read_buffer[iter] *
                 *(cs_object_reference->sensor_handle->adc_ref_volt) * VOLT_TO_MILLIVOLT) /
                (VT_FLOAT)pow(2, *(cs_object_reference->sensor_handle->adc_resolution))) *
            (*(cs_object_reference->sensor_handle->currentsense_mV_to_mA));

        raw_signature_buffer->num_datapoints++;
        //printf("num_datapoints - %d ",raw_signature_buffer->num_datapoints);
        if (raw_signature_buffer->num_datapoints == raw_signature_buffer->sample_length)
        {   //HEREprintf("full \n");
            //HEREprintf("for sampling_frequency - %d \n",(int)raw_signature_buffer->sampling_frequency);
            return RAW_SIGNATURE_BUFFER_FILLED;
        }
    }
    return RAW_SIGNATURE_BUFFER_NOT_FILLED;
}

static VT_VOID cs_downsample_non_repeating_raw_signature_buffer()
{
    VT_UINT samples_stored = cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_datapoints;
    VT_FLOAT downsample_factor=1;
    /* Don't downsample if one round of data collection / two rounds of half buffer collection is not done */
    if (!(samples_stored > (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length / 2)))
    {
        return;
    }

    VT_FLOAT downsample_factor_for_prev_cycle =
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency / 
            cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency;

    VT_FLOAT next_cycle_data_points=
        (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length / 2)/downsample_factor_for_prev_cycle;

    if((next_cycle_data_points+cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_datapoints)
        >=cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length)
    {

    downsample_factor =
        (next_cycle_data_points+cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_datapoints)/
            cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length; 

    }
    /*
    VT_FLOAT downsample_factor =
        ((cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_adc_buffer_datapoints_iterated /
             (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length / 2)) +
            1) /
        (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_adc_buffer_datapoints_iterated /
            (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sample_length / 2));
    */
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
    //int sampling_frequency_temp;
    //sampling_frequency_temp=(int)cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency;
    //printf("sampling_frequency_temp before %d \n",sampling_frequency_temp);

    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.num_datapoints = samples_stored_after_modification;

    //printf("samples_stored_after_modification %d \n",samples_stored_after_modification);
    /*
    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency / downsample_factor;
    //printf("adc_read_sampling_frequency %d \n",(int)cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency);
    */

    cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency / downsample_factor;

    //HEREprintf("sampling_frequency_uno- %d",(int)cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency);
    //printf("adc_read_sampling_frequency %d \n",(int)cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency);

    //sampling_frequency_temp=(int)cs_object_reference->raw_signatures_reader->non_repeating_raw_signature.sampling_frequency;
    //printf("sampling_frequency_temp after %d \n",sampling_frequency_temp);
}

static VT_BOOL cs_adc_buffer_to_repeating_raw_signature_buffers(VT_UINT adc_read_buffer_start_index)
{   //printf("repeating cs_adc_buffer \n");
    VT_BOOL all_raw_signature_buffers_filled = true;
    for (VT_UINT iter = 0; iter < cs_object_reference->raw_signatures_reader->num_repeating_raw_signatures; iter++)
<<<<<<< HEAD
    {   //HEREprintf("rep \n");
        //in this expression, all_raw_signature_buffers_filled = all_raw_signature_buffers_filled && ()
        //once all_raw_signature_buffers_filled is false, cs_downsample_half_adc_buffer fucntion is not called again
        //which causes the code to run serially and not paralelley
        //thats why rep is called but cs_downsample_half_adc_buffer is not
        //hence 5000, 78 and 12 is being called serially.
=======
    {   
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
        bool flag= cs_downsample_half_adc_buffer(
                &cs_object_reference->raw_signatures_reader->repeating_raw_signatures[iter], adc_read_buffer_start_index);
        all_raw_signature_buffers_filled =all_raw_signature_buffers_filled && flag;
            
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
    // VTLogDebug("cs_downsample_non_repeating_raw_signature_buffer() \r\n");
    cs_downsample_non_repeating_raw_signature_buffer();

    /* Store new datapoints */
    // VTLogDebug("cs_downsample_half_adc_buffer() \r\n");
    //HEREprintf("non_rep \n");
    cs_downsample_half_adc_buffer(
        &cs_object_reference->raw_signatures_reader->non_repeating_raw_signature, adc_read_buffer_start_index);
}

static VT_VOID cs_raw_signature_read_half_complete_callback()
{   //printf("half callback\n");
    // VTLogDebug("cs_raw_signature_read_half_complete_callback() \r\n");
    VT_BOOL repeating_raw_signature_buffers_filled = false;

    if (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature_stop_collection &&
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled)
    {
        return;
    }

    /* Transfer data from adc buffer to non repeating signature buffer */
    // VTLogDebug("cs_adc_buffer_to_non_repeating_raw_signature_buffer() \r\n");
    //printf("non_repeat \n");
    cs_adc_buffer_to_non_repeating_raw_signature_buffer(0);

    if (cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled)
    {
        return;
    }

    /* Transfer data from adc buffer to repeating signature buffers */
    //printf("repeat \n");
    repeating_raw_signature_buffers_filled = cs_adc_buffer_to_repeating_raw_signature_buffers(0);
    if (repeating_raw_signature_buffers_filled)
    {
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    }
}

static VT_VOID cs_raw_signature_read_full_complete_callback()
{   //printf("full callback\n");
    // VTLogDebug("cs_raw_signature_read_full_complete_callback() \r\n");
    VT_BOOL repeating_raw_signature_buffers_filled = false;

    if (cs_object_reference->raw_signatures_reader->non_repeating_raw_signature_stop_collection &&
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled)
    {
        return;
    }

    /* Restart current acquisition */
    // VTLogDebug("adc_buffer_read() \r\n");
    cs_object_reference->device_driver->adc_buffer_read(cs_object_reference->sensor_handle->adc_id,
        cs_object_reference->sensor_handle->adc_controller,
        cs_object_reference->sensor_handle->adc_channel,
        cs_object_reference->raw_signatures_reader->adc_read_buffer,
        VT_CS_SAMPLE_LENGTH,
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        &cs_raw_signature_read_half_complete_callback,
        &cs_raw_signature_read_full_complete_callback);

    /* Transfer data from adc buffer to non repeating signature buffer */
    // VTLogDebug("cs_adc_buffer_to_non_repeating_raw_signature_buffer() \r\n");
    cs_adc_buffer_to_non_repeating_raw_signature_buffer(VT_CS_SAMPLE_LENGTH / 2);

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

static VT_UINT find_closest_floor_stored_sampling_frequency(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT desired_sampling_frequency, VT_FLOAT* closest_floor_sampling_frequency)
{
    VT_FLOAT min_diff = 65535;
<<<<<<< HEAD
    //VT_FLOAT min_freq=65535;

    // for (VT_UINT iter1 = 0; iter1 < cs_object->raw_signatures_reader->num_repeating_raw_signatures; iter1++){
    //     if (cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].sampling_frequency < min_freq){
    //         min_freq=cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].sampling_frequency;
    //     }
    // }

    // if(desired_sampling_frequency<min_freq){
        
    //     *closest_floor_sampling_frequency =VT_CS_FMIN;
    //     return VT_SUCCESS;
    // }
=======
    VT_FLOAT min_freq=65535;

    for (VT_UINT iter1 = 0; iter1 < cs_object->raw_signatures_reader->num_repeating_raw_signatures; iter1++){
        if (cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].sampling_frequency < min_freq){
            min_freq=cs_object->raw_signatures_reader->repeating_raw_signatures[iter1].sampling_frequency;
        }
    }

    if(desired_sampling_frequency<min_freq){
        ;
        *closest_floor_sampling_frequency =min_freq;
        return VT_SUCCESS;
    }
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3

    for (VT_UINT iter = 0; iter < cs_object->raw_signatures_reader->num_repeating_raw_signatures; iter++)
    {
        if (desired_sampling_frequency >= cs_object->raw_signatures_reader->repeating_raw_signatures[iter].sampling_frequency)
        {
            if (min_diff > (desired_sampling_frequency -
                               cs_object->raw_signatures_reader->repeating_raw_signatures[iter].sampling_frequency))
            {
                min_diff = desired_sampling_frequency -
                           cs_object->raw_signatures_reader->repeating_raw_signatures[iter].sampling_frequency;
                *closest_floor_sampling_frequency =
                    cs_object->raw_signatures_reader->repeating_raw_signatures[iter].sampling_frequency;
            }
        }
    }
    if (min_diff == 65535)
    {   printf("error in closest floor");
        return VT_ERROR;
    }
    return VT_SUCCESS;
}

static VT_UINT upsample_stored_current_measurement(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* extrapolated_repeating_raw_signature,
    VT_FLOAT desired_sampling_frequency,
    VT_FLOAT closest_floor_sampling_frequency,
    VT_UINT desired_sample_length)
{
    VT_FLOAT upsample_factor           = closest_floor_sampling_frequency / desired_sampling_frequency;
    VT_UINT upsampled_buffer_iter      = 0;
    VT_FLOAT raw_signature_buffer_iter = 0;

    for (VT_UINT iter = 0; iter < cs_object->raw_signatures_reader->num_repeating_raw_signatures; iter++)
    {
        if (closest_floor_sampling_frequency ==
            cs_object->raw_signatures_reader->repeating_raw_signatures[iter].sampling_frequency)
        {
            while (upsampled_buffer_iter < desired_sample_length)
            {
                extrapolated_repeating_raw_signature[upsampled_buffer_iter] =
                    cs_object->raw_signatures_reader->repeating_raw_signatures[iter]
                        .current_measured[(VT_UINT)raw_signature_buffer_iter];
                upsampled_buffer_iter++;
                raw_signature_buffer_iter += upsample_factor;
            }
            return VT_SUCCESS;
        }
    }
    return VT_ERROR;
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

    /* If number of repeating signature sampling frequencies is zero set repeating_raw_signature_ongoing_collection to false and
     * repeating_raw_signature_buffers_filled to true*/
    if (num_repeating_signature_sampling_frequencies == 0)
    {
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
        cs_object_reference->raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    }

    /* Set ADC sampling freqeuncy to max sampling frequency configured*/
    cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency = VT_CS_ADC_MAX_SAMPLING_FREQ;

    /* Initialize buffers of repeating raw signatures array */
    for (VT_UINT iter = 0; iter < num_repeating_signature_sampling_frequencies; iter++)
    {
        cs_raw_signature_buffer_init(&(cs_object_reference->raw_signatures_reader->repeating_raw_signatures[iter]),
            repeating_signature_sampling_frequencies[iter],
            sample_length);
            //HEREprintf("repeating_signature_sampling_frequencies - %d \n",(int)repeating_signature_sampling_frequencies[iter]);
    }

    /* Initialize buffer of non-repeating raw signature*/
    cs_raw_signature_buffer_init(&(cs_object_reference->raw_signatures_reader->non_repeating_raw_signature),
        cs_object_reference->raw_signatures_reader->adc_read_sampling_frequency,
        sample_length);

<<<<<<< HEAD
 //cs_object_reference->raw_signatures_reader->num_repeating_raw_signatures=1;
=======
 
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
    // VTLogDebug("adc_buffer_read() \r\n");
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

VT_UINT cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* extrapolated_repeating_raw_signature,
    VT_FLOAT desired_sampling_frequency,
    VT_UINT desired_sample_length)
{
    VT_FLOAT closest_floor_sampling_frequency = 0;

    /* Check whether the shared buffer size is sufficent and has been initialized correctly */
    if (cs_object->raw_signatures_reader_initialized == false)
    {
        return VT_ERROR;
    }

    if (find_closest_floor_stored_sampling_frequency(cs_object, desired_sampling_frequency, &closest_floor_sampling_frequency) ==
        VT_SUCCESS)
    { 
        return upsample_stored_current_measurement(cs_object,
            extrapolated_repeating_raw_signature,
            desired_sampling_frequency,
            closest_floor_sampling_frequency,
            desired_sample_length);
    }
    
    return VT_ERROR;
}

VT_UINT cs_non_repeating_raw_signature_fetch_stored_current_measurement(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* non_repeating_raw_signature,
    VT_FLOAT* sampling_frequency,
    VT_UINT* num_datapoints)
{
    /* Check whether the shared buffer size is sufficent and has been initialized correctly */
    if (cs_object->raw_signatures_reader_initialized == false)
    {
        return VT_ERROR;
    }

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
