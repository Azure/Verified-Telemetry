/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <stdbool.h>

#include "vt_fc_read.h"
#include "vt_debug.h"

#define MAX_TICK_VALUE 65535
#define MIN_TICK_VALUE 1
#define MIN_TICK_RESOLUTION 1

static VT_VOID fc_calculate_required_tick_resolution(
    VT_ULONG sampling_interval_us, VT_UINT* tick_resolution_us, VT_UINT max_tick_value)
{
    VT_FLOAT sampling_period_ticks;
    if(*tick_resolution_us < MIN_TICK_RESOLUTION)
    {
        *tick_resolution_us = MIN_TICK_RESOLUTION;
    }
    if(max_tick_value < MIN_TICK_VALUE)
    {
        max_tick_value = MAX_TICK_VALUE;
    }
    while (true)
    {
        sampling_period_ticks = (VT_FLOAT)sampling_interval_us / (VT_FLOAT)*tick_resolution_us;
        if (sampling_period_ticks < (VT_FLOAT)max_tick_value)
        {
            break;
        }
        *tick_resolution_us = *tick_resolution_us + 1;
    }
}

VT_VOID fc_adc_read(VT_FALLCURVE_OBJECT* fc_object, VT_UINT* raw_signature, VT_ULONG sampling_interval_us, VT_UINT sample_length)
{
    VT_UINT max_tick_value        = 0;
    VT_UINT tick_resolution_usec  = 0;
    VT_UINT adc_resolution        = 0;
    VT_FLOAT adc_ref_volt         = 0;
    VT_UINT start_tick_count      = 0;
    VT_UINT sampling_period_ticks = 0;

    fc_object->device_driver->tick_init(&max_tick_value, &tick_resolution_usec);
    fc_calculate_required_tick_resolution(sampling_interval_us, &tick_resolution_usec, max_tick_value);
    sampling_period_ticks = round((float)sampling_interval_us / (float)tick_resolution_usec);
    VTLogDebug("Sampling Period Ticks: %d \r\n", sampling_period_ticks);

    fc_object->device_driver->adc_init(fc_object->sensor_handle->adc_id,
        fc_object->sensor_handle->adc_controller,
        fc_object->sensor_handle->adc_channel,
        &adc_resolution,
        &adc_ref_volt);
    fc_object->device_driver->interrupt_disable();
    fc_object->device_driver->tick_init(&max_tick_value, &tick_resolution_usec);
    fc_object->device_driver->gpio_off(
        fc_object->sensor_handle->gpio_id, fc_object->sensor_handle->gpio_port, fc_object->sensor_handle->gpio_pin);
    start_tick_count = (VT_UINT)fc_object->device_driver->tick();

    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        raw_signature[iter] = fc_object->device_driver->adc_read(
            fc_object->sensor_handle->adc_id, fc_object->sensor_handle->adc_controller, fc_object->sensor_handle->adc_channel);
        while ((uint16_t)((VT_UINT)fc_object->device_driver->tick() - start_tick_count) < sampling_period_ticks)
        {
        }
        start_tick_count = (VT_UINT)fc_object->device_driver->tick();
    }

    fc_object->device_driver->tick_deinit();
    fc_object->device_driver->interrupt_enable();
    fc_object->device_driver->gpio_on(
        fc_object->sensor_handle->gpio_id, fc_object->sensor_handle->gpio_port, fc_object->sensor_handle->gpio_pin);
}