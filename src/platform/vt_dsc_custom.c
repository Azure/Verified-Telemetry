/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_dsc.h"

uint32_t  _vt_dsc_delay_msec(uint32_t delay)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_gpio_read(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, int *state)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel, float* value)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}


uint32_t  _vt_dsc_flash_erase(uint32_t flashAddress, uint32_t Nsize)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_flash_write(uint32_t flashAddress, void* wrBuf, uint32_t Nsize)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t  _vt_dsc_flash_read(uint32_t flashAddress, void* rdBuf, uint32_t Nsize)
{
    return VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED;
}