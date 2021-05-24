/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef _VT_PLATFORM_H
#define _VT_PLATFORM_H

#include "vt_defs.h"

typedef VT_UINT (*VT_ADC_FUNC_INIT)(VT_ADC_ID adc_id,
    VT_ADC_CONTROLLER* adc_controller,
    VT_ADC_CHANNEL* adc_channel,
    VT_UINT* adc_resolution,
    VT_FLOAT* adc_ref_volt);
typedef VT_UINT (*VT_ADC_FUNC)(VT_ADC_ID adc_id, VT_ADC_CONTROLLER* adc_controller, VT_ADC_CHANNEL* adc_channel);
typedef VT_UINT (*VT_GPIO_FUNC)(VT_GPIO_ID gpio_id, VT_GPIO_PORT* gpio_port, VT_GPIO_PIN* gpio_pin);
typedef VT_UINT (*VT_TICK_INIT_FUNC)(VT_UINT* max_value, VT_UINT* resolution_usec);
typedef VT_ULONG (*VT_TICK_FUNC)();
typedef VT_VOID (*VT_INTERRUPT_CTRL)();

typedef struct VT_DEVICE_DRIVER_STRUCT
{
    VT_ADC_FUNC_INIT adc_init;
    VT_ADC_FUNC adc_read;
    VT_GPIO_FUNC gpio_on;
    VT_GPIO_FUNC gpio_off;
    VT_TICK_INIT_FUNC tick_init;
    VT_TICK_FUNC tick_deinit;
    VT_TICK_FUNC tick;
    VT_INTERRUPT_CTRL interrupt_enable;
    VT_INTERRUPT_CTRL interrupt_disable;
} VT_DEVICE_DRIVER;

typedef struct VT_SENSOR_HANDLE_STRUCT
{
    VT_GPIO_ID gpio_id;
    VT_GPIO_PORT* gpio_port;
    VT_GPIO_PIN* gpio_pin;
    VT_ADC_ID adc_id;
    VT_ADC_CONTROLLER* adc_controller;
    VT_ADC_CHANNEL* adc_channel;
} VT_SENSOR_HANDLE;

#endif
