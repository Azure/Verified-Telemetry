/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_DSC_H
#define _VT_DSC_H

#include "vt_api.h"
#include "vt_port.h"

#define VT_FLASH_ADDRESS_ERROR    0X16
#define VT_FLASH_WRITE_SIZE_ERROR 0X17
#define VT_FLASH_ERAZE_SIZE_ERROR 0X18

uint32_t _vt_dsc_delay_msec(uint32_t delay);

uint32_t _vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay);

uint32_t _vt_dsc_gpio_read(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, int* state);

uint32_t _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin);

uint32_t _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin);

uint32_t _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel, uint32_t* value);

uint32_t _vt_dsc_flash_erase(uint32_t flashAddress, uint32_t Nsize);

uint32_t _vt_dsc_flash_write(uint32_t flashAddress, void* wrBuf, uint32_t Nsize);

uint32_t _vt_dsc_flash_read(uint32_t flashAddress, void* rdBuf, uint32_t Nsize);

#endif
