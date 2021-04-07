/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_DSC_H
#define _VT_DSC_H

#include <stdint.h>

#include "vt_port.h"

#define VT_PLATFORM_SUCCESS                            0X00
#define VT_PLATFORM_IMPLEMENTATION_NOT_PROVIDED        0X01

uint32_t _vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay);

uint32_t _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin);

uint32_t _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin);

uint32_t _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* adc_controller, ADC_CHANNEL_TYPEDEF adc_channel, uint32_t* value);

#endif
