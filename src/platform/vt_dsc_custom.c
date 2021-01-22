/* Copyright (c) Microsoft Corporation. */

#include "vt_dsc.h"

uint _vt_dsc_gpio_read(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, int *state)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel, float* value)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}
