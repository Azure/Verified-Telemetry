/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>

#include "vt_debug.h"
#include "vt_fc_signature.h"

VT_UINT fc_signature_compute_collection_settings(
    VT_FALLCURVE_OBJECT* fc_object, VT_ULONG* sampling_interval_us, VT_UINT8* confidence_metric)
{
    VTLogInfo("\tComputing FallCurve Collection Settings\n");

    VT_UINT max_tick_value            = 65535;
    VT_UINT tick_resolution_usec      = 1;
    VT_ULONG time_to_fall             = 0;
    VT_ULONG time_to_powerup          = 0;
    const VT_ULONG max_time_allowed   = VT_FC_MAX_SAMPLING_INTERVAL_US * VT_FC_SAMPLE_LENGTH;
    VT_UINT adc_value_start           = 0;
    VT_UINT adc_value                 = 0;
    VT_UINT start_tick_count          = 0;
    VT_UINT adc_resolution            = 0;
    VT_FLOAT adc_ref_volt             = 0;
    VT_ULONG falltime_calib_test      = 0;
    VT_FLOAT pearson_coeff_calib_test = 0;
    VT_UINT falltime_datapoints       = 0;

    fc_object->device_driver->adc_init(fc_object->sensor_handle->adc_id,
        fc_object->sensor_handle->adc_controller,
        fc_object->sensor_handle->adc_channel,
        &adc_resolution,
        &adc_ref_volt);
    adc_value_start = fc_object->device_driver->adc_read(
        fc_object->sensor_handle->adc_id, fc_object->sensor_handle->adc_controller, fc_object->sensor_handle->adc_channel);
    VTLogDebug("FallCurve first value: %d \r\n", adc_value_start);
    adc_value_start = round(adc_value_start * 0.37f);

    fc_object->device_driver->interrupt_disable();
    fc_object->device_driver->tick_init(&max_tick_value, &tick_resolution_usec);
    fc_object->device_driver->gpio_off(
        fc_object->sensor_handle->gpio_id, fc_object->sensor_handle->gpio_port, fc_object->sensor_handle->gpio_pin);
    start_tick_count = (VT_UINT)fc_object->device_driver->tick();

    while (time_to_fall < max_time_allowed)
    {
        adc_value = fc_object->device_driver->adc_read(
            fc_object->sensor_handle->adc_id, fc_object->sensor_handle->adc_controller, fc_object->sensor_handle->adc_channel);
        if (adc_value <= adc_value_start)
        {
            break;
        }
        time_to_fall += (VT_UINT)((VT_UINT)fc_object->device_driver->tick() - start_tick_count);
        start_tick_count = (VT_UINT)fc_object->device_driver->tick();
    }

    fc_object->device_driver->tick_deinit();
    fc_object->device_driver->interrupt_enable();
    fc_object->device_driver->gpio_on(
        fc_object->sensor_handle->gpio_id, fc_object->sensor_handle->gpio_port, fc_object->sensor_handle->gpio_pin);

    VTLogDebug("FallCurve expected last value: %d \r\n", adc_value_start);
    VTLogDebug("FallCurve last value: %d \r\n", adc_value);
    VTLogDebug("FallCurve time to reach 0.37: %lu \r\n", time_to_fall);
    VTLogDebug("Maximum time allowed: %lu \r\n", max_time_allowed);

    if (time_to_fall > max_time_allowed)
    {
        *confidence_metric    = 0;
        *sampling_interval_us = VT_FC_MAX_SAMPLING_INTERVAL_US;
    }
    else if (time_to_fall == 0)
    {
        *confidence_metric    = 0;
        *sampling_interval_us = VT_FC_MIN_SAMPLING_INTERVAL_US;
    }
    else
    {
        *confidence_metric    = 100;
        *sampling_interval_us = round(time_to_fall / 100.0f);
    }
    if (*sampling_interval_us < VT_FC_MIN_SAMPLING_INTERVAL_US)
    {
        *sampling_interval_us = VT_FC_MIN_SAMPLING_INTERVAL_US;
    }
    VTLogDebug("Sampling Interval: %lu \r\n", *sampling_interval_us);

    fc_object->device_driver->interrupt_disable();
    fc_object->device_driver->tick_init(&max_tick_value, &tick_resolution_usec);
    start_tick_count = (VT_UINT)fc_object->device_driver->tick();
    while (time_to_powerup < time_to_fall)
    {
        time_to_powerup += (VT_UINT)((VT_UINT)fc_object->device_driver->tick() - start_tick_count);
        start_tick_count = (VT_UINT)fc_object->device_driver->tick();
    }
    fc_object->device_driver->tick_deinit();
    fc_object->device_driver->interrupt_enable();

    if (fc_signature_compute(fc_object, *sampling_interval_us, &falltime_calib_test, &pearson_coeff_calib_test))
    {
        return VT_ERROR;
    }

    falltime_datapoints   = round((float)falltime_calib_test / (float)*sampling_interval_us);
    *sampling_interval_us = round(((float)(*sampling_interval_us) * (float)falltime_datapoints) / (float)VT_FC_SAMPLE_LENGTH);
    if (*sampling_interval_us < VT_FC_MIN_SAMPLING_INTERVAL_US)
    {
        *sampling_interval_us = VT_FC_MIN_SAMPLING_INTERVAL_US;
    }

    return VT_SUCCESS;
}
