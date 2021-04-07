/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_dsc.h"

uint32_t _vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay)
{
    return VT_PLATFORM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin)
{
    return VT_PLATFORM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin)
{
    return VT_PLATFORM_IMPLEMENTATION_NOT_PROVIDED;
}

uint32_t _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* adc_controller, ADC_CHANNEL_TYPEDEF adc_channel, uint32_t* value)
{
    return VT_PLATFORM_IMPLEMENTATION_NOT_PROVIDED;
}