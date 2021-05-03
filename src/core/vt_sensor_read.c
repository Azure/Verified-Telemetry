/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "vt_database.h"
#include "vt_dsc.h"
#include "vt_sensor.h"
#include "vt_debug.h"

uint32_t vt_sensor_read_value(VT_SENSOR* sensor_ptr, uint32_t* sensor_value)
{
    if (sensor_ptr == NULL)
    {
        return VT_PTR_ERROR;
    }

    return _vt_dsc_adc_read(sensor_ptr->vt_adc_controller, sensor_ptr->vt_adc_channel, sensor_value);
}

uint32_t vt_sensor_read_fingerprint(
    VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, char* fingerprint_string, uint32_t fingerprint_string_lenth)
{
    if (sensor_ptr == NULL)
    {
        return VT_PTR_ERROR;
    }    
    
    if (fingerprint_string_lenth < 3 * VT_FINGERPRINT_LENGTH)
    {
        return VT_ERROR;
    }

    char buffer[10] = "";
    strcpy(fingerprint_string, "");

    uint32_t status = _vt_sensor_read_fingerprint(sensor_ptr, fingerprint_array, sensor_ptr->vt_sampling_frequency);

    for (uint8_t i = 0; i < VT_FINGERPRINT_LENGTH; i++)
    {
        snprintf(buffer, sizeof(buffer), "%d", (int16_t)fingerprint_array[i]);
        strcat(fingerprint_string, buffer);

        if (i < VT_FINGERPRINT_LENGTH - 1)
        {
            strcat(fingerprint_string, ",");
        }
    }

    return status;
}

uint32_t vt_sensor_read_status(
    VT_SENSOR* sensor_ptr, VT_DATABASE* database_ptr, uint32_t* fingerprint, int8_t* sensor_id)
{
    uint32_t fall_time;
    float pearson_coefficient;

    *sensor_id = 0;
    #if VT_LOG_LEVEL > 2
        int32_t decimal;
        float frac_float;
        int32_t frac;
    #endif /* VT_LOG_LEVEL > 2 */

    if (_vt_database_check_pearson_falltime_availability(database_ptr))
    {
        *sensor_id = -1;
        return VT_SUCCESS;
    }

    if (_vt_fingerprint_calculate_falltime_pearsoncoefficient(
            fingerprint, VT_FINGERPRINT_LENGTH, sensor_ptr->vt_sampling_frequency, &fall_time, &pearson_coefficient) ==
        VT_SUCCESS)
    {
        VTLogDebug("FallTime: %lu \r\n", fall_time);
        #if VT_LOG_LEVEL > 2
            decimal = pearson_coefficient;
            frac_float = pearson_coefficient - (float)decimal;
            frac = frac_float * 10000; 
        #endif /* VT_LOG_LEVEL > 2 */
        VTLogDebug("Pearson Coeff: %lu.%lu \r\n", decimal, frac);
        *sensor_id = _vt_database_evaluate_pearson_falltime(database_ptr, fall_time, pearson_coefficient);

        return VT_SUCCESS;
    }

    return VT_ERROR;
}

static uint32_t _calculate_required_tick_resolution(uint16_t sampling_frequency, uint16_t* tick_resolution_usec, uint16_t max_tick_value)
{
   float sampling_period_ticks;
   if(sampling_frequency == 0)
    {
        return 1;
    }
   while(true)
    {
        sampling_period_ticks = ((float)sampling_frequency)/((float)*tick_resolution_usec);
        if(sampling_period_ticks < (float)max_tick_value)
        {
            break;
        }
        *tick_resolution_usec = *tick_resolution_usec + 1;
    }
    return 0;
}

uint32_t _vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, uint16_t sampling_frequency)
{
    uint32_t status;

    status = _vt_dsc_gpio_turn_off(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_PLATFORM_SUCCESS)
    {
        return status;
    }

    uint16_t max_tick_value = 65535;
    uint16_t tick_resolution_usec = 1;
    _calculate_required_tick_resolution(sampling_frequency, &tick_resolution_usec, max_tick_value);
    _vt_dsc_tick_init(sensor_ptr->vt_timer, &max_tick_value, &tick_resolution_usec);
    unsigned long sampling_period_ticks = round((float)sampling_frequency/(float)tick_resolution_usec);
    unsigned long start_tick_count = _vt_dsc_tick(sensor_ptr->vt_timer);
    unsigned long current_tick_count = _vt_dsc_tick(sensor_ptr->vt_timer);
    for (uint8_t i = 0; i < VT_FINGERPRINT_LENGTH; i++)
    {
        status = _vt_dsc_adc_read(sensor_ptr->vt_adc_controller, sensor_ptr->vt_adc_channel, &fingerprint_array[i]);
        if (status != VT_PLATFORM_SUCCESS)
        {
            return status;
        }
        current_tick_count = _vt_dsc_tick(sensor_ptr->vt_timer);
        while(current_tick_count - start_tick_count < sampling_period_ticks){
        current_tick_count = _vt_dsc_tick(sensor_ptr->vt_timer);
        }
        start_tick_count = _vt_dsc_tick(sensor_ptr->vt_timer);
    }
    _vt_dsc_tick_deinit(sensor_ptr->vt_timer);
    
    status = _vt_dsc_gpio_turn_on(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_PLATFORM_SUCCESS)
    {
        return status;
    }

    return status;
}
