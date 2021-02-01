/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "string.h"
#include "vt_sensor.h"

uint32_t vt_sensor_initialize(VT_SENSOR* sensor_ptr,
    char* Port_Name,
    GPIO_PORT_TYPEDEF* GPIOx,
    GPION_PIN_TYPEDEF GPIO_Pin,
    ADC_CONTROLLER_TYPEDEF* ADC_Controller,
    ADC_CHANNEL_TYPEDEF ADC_Channel,
    TIMER_HANDLE_TYPEDEF* Timer_Handler)
{
    if (sensor_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    memset((void*)sensor_ptr, 0, sizeof(VT_SENSOR));

    sensor_ptr->vt_sensor_name    = strdup(Port_Name);
    sensor_ptr->vt_gpio_port      = GPIOx;
    sensor_ptr->vt_gpio_pin       = GPIO_Pin;
    sensor_ptr->vt_adc_controller = ADC_Controller;
    sensor_ptr->vt_adc_channel    = ADC_Channel;

    sensor_ptr->vt_timer = Timer_Handler;

    vt_sensor_calibrate(sensor_ptr);

    return (VT_SUCCESS);
}
