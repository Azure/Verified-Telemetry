/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_database.h"
#include "vt_dsc.h"
#include "vt_sensor.h"

uint32_t vt_sensor_read_value(VT_SENSOR* sensor_ptr, uint32_t* sensor_value)
{
    if (sensor_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    return _vt_dsc_adc_read(sensor_ptr->vt_adc_controller, sensor_ptr->vt_adc_channel, sensor_value);
}

uint32_t vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, char* fingerprint_string)
{
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

    if (_vt_database_check_pearson_falltime_availability(database_ptr))
    {
        *sensor_id = -1;
        return VT_SUCCESS;
    }

    if (_vt_fingerprint_calculate_falltime_pearsoncoefficient(
            fingerprint, VT_FINGERPRINT_LENGTH, sensor_ptr->vt_sampling_frequency, &fall_time, &pearson_coefficient) ==
        VT_SUCCESS)
    {
        *sensor_id = _vt_database_evaluate_pearson_falltime(database_ptr, fall_time, pearson_coefficient);

        return VT_SUCCESS;
    }

    return VT_ERROR;
}

uint32_t _vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, uint16_t sampling_frequency)
{
    if (sensor_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    uint32_t status;

    status = _vt_dsc_gpio_turn_off(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_PLATFORM_SUCCESS)
    {
        return status;
    }

    for (uint8_t i = 0; i < VT_FINGERPRINT_LENGTH; i++)
    {
        status = _vt_dsc_adc_read(sensor_ptr->vt_adc_controller, sensor_ptr->vt_adc_channel, &fingerprint_array[i]);
        if (status != VT_PLATFORM_SUCCESS)
        {
            return status;
        }

        status = _vt_dsc_delay_usec(sensor_ptr->vt_timer, sampling_frequency);
        if (status != VT_PLATFORM_SUCCESS)
        {
            return status;
        }
    }

    status = _vt_dsc_gpio_turn_on(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_PLATFORM_SUCCESS)
    {
        return status;
    }

    return status;
}
