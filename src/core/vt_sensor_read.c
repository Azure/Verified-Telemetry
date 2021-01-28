/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_sensor.h"
#include "vt_database.h"
#include "vt_dsc.h"


uint vt_sensor_read_value(VT_SENSOR* sensor_ptr, uint32_t* sensor_value)
{
    if (sensor_ptr == NULL)
        return (VT_PTR_ERROR);

    uint status;

    status = _vt_dsc_gpio_turn_on(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_SUCCESS)
        return status;

    // tx_mutex_get(&sensor_ptr->vt_mutex_ptr, TX_WAIT_FOREVER);
    status = _vt_dsc_adc_read(sensor_ptr->vt_adc_controller, sensor_ptr->vt_adc_channel, sensor_value);
    // tx_mutex_put(&sensor_ptr->vt_mutex_ptr);

    return (status);
}

uint vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, char* fingerprint_string)
{
    char buffer[10] = "";
    strcpy(fingerprint_string, "");

    int status = _vt_sensor_read_fingerprint(sensor_ptr, fingerprint_array, sensor_ptr->vt_sampling_frequency);

    for (int i = 0; i < 100; i++)
    {
        snprintf(buffer,sizeof(buffer),"%d",(int)fingerprint_array[i]);
        strcat(fingerprint_string, buffer);

        if (i < 99)
            strcat(fingerprint_string, ",");
    }

    return status;
}

uint vt_sensor_read_status(VT_SENSOR* sensor_ptr, VT_DATABASE* database_ptr, uint32_t* fingerprint, int* sensorid)
{
    int fall_time;
    double pearson_coefficient;

    *sensorid = 0;

    if (_vt_fingerprint_calculate_falltime_pearsoncoefficient(fingerprint, 100, sensor_ptr->vt_sampling_frequency, &fall_time, &pearson_coefficient) == VT_SUCCESS) {
        int sensorid_ftpc = _vt_database_evaluate_pearson_falltime(database_ptr, fall_time, pearson_coefficient);
        printf("\nStatus with FTPC = %d \n", sensorid_ftpc);

        *sensorid = sensorid_ftpc;

        return VT_SUCCESS;
    }
    
    return VT_SUCCESS;
}

uint _vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, int sampling_frequency)
{
    if (sensor_ptr == NULL)
        return (VT_PTR_ERROR);

    uint status;

    status = _vt_dsc_gpio_turn_off(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_SUCCESS)
        return status;

    for (int i = 0; i < 100; i++)
    {
        // tx_mutex_get(&sensor_ptr->vt_mutex_ptr, TX_WAIT_FOREVER);
        status = _vt_dsc_adc_read(sensor_ptr->vt_adc_controller, sensor_ptr->vt_adc_channel, &fingerprint_array[i]);
        // tx_mutex_put(&sensor_ptr->vt_mutex_ptr);
        if (status != VT_SUCCESS)
            return status;

        if (sensor_ptr->vt_timer != NULL)
            _vt_dsc_delay_usec(sensor_ptr->vt_timer,sampling_frequency);
        else
            _vt_dsc_delay_msec(sampling_frequency);

        if (status != VT_SUCCESS)
            return status;
    }

    status = _vt_dsc_gpio_turn_on(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);
    if (status != VT_SUCCESS)
        return status;

    return (status);
}