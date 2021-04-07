/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "string.h"
#include "vt_sensor.h"
#include "vt_dsc.h"

uint32_t vt_sensor_initialize(VT_SENSOR* sensor_ptr,
    char* sensor_name,
    GPIO_PORT_TYPEDEF* gpio_port,
    GPIO_PIN_TYPEDEF gpio_pin,
    ADC_CONTROLLER_TYPEDEF* adc_controller,
    ADC_CHANNEL_TYPEDEF adc_channel,
    TIMER_HANDLE_TYPEDEF* timer_handler)
{
    if (sensor_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    memset((void*)sensor_ptr, 0, sizeof(VT_SENSOR));

    sensor_ptr->vt_sensor_name        = strdup(sensor_name);
    sensor_ptr->vt_gpio_port          = gpio_port;
    sensor_ptr->vt_gpio_pin           = gpio_pin;
    sensor_ptr->vt_adc_controller     = adc_controller;
    sensor_ptr->vt_adc_channel        = adc_channel;
    sensor_ptr->vt_timer              = timer_handler;
    sensor_ptr->vt_sampling_frequency = 1;

    return _vt_dsc_gpio_turn_on(sensor_ptr->vt_gpio_port, sensor_ptr->vt_gpio_pin);;
}
