/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_PORT_H
#define _VT_PORT_H

#include "stdint.h"

#define CUSTOM_DEVICE

#ifdef STM32F4XX
#undef CUSTOM_DEVICE
#include "stm32f4xx_hal.h"
#define GPIO_PORT_TYPEDEF      GPIO_TypeDef
#define GPIO_PIN_TYPEDEF      uint16_t
#define ADC_CONTROLLER_TYPEDEF ADC_HandleTypeDef
#define ADC_CHANNEL_TYPEDEF    uint32_t
#define TIMER_HANDLE_TYPEDEF   TIM_HandleTypeDef
#endif

#ifdef STM32L4XX
#undef CUSTOM_DEVICE
#include "stm32l4xx_hal.h"
#define GPIO_PORT_TYPEDEF      GPIO_TypeDef
#define GPIO_PIN_TYPEDEF      uint16_t
#define ADC_CONTROLLER_TYPEDEF ADC_HandleTypeDef
#define ADC_CHANNEL_TYPEDEF    uint32_t
#define TIMER_HANDLE_TYPEDEF   TIM_HandleTypeDef
#endif

#ifdef CUSTOM_DEVICE
#define GPIO_PORT_TYPEDEF      uint16_t
#define GPIO_PIN_TYPEDEF      uint16_t
#define ADC_CONTROLLER_TYPEDEF uint16_t
#define ADC_CHANNEL_TYPEDEF    uint16_t
#define TIMER_HANDLE_TYPEDEF   uint16_t
#endif


#endif
