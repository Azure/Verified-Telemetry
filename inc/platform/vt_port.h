/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_PORT_H
#define _VT_PORT_H

#ifdef STM32F4XX
#include "stm32f4xx_hal.h"
#endif
#ifdef STM32L4XX
#include "stm32l4xx_hal.h"
#endif

#ifdef STM32F4XX
#define GPIO_PORT_TYPEDEF      GPIO_TypeDef
#define GPION_PIN_TYPEDEF      uint16_t
#define ADC_CONTROLLER_TYPEDEF ADC_HandleTypeDef
#define ADC_CHANNEL_TYPEDEF    uint32_t
#define TIMER_HANDLE_TYPEDEF   TIM_HandleTypeDef
#endif

#ifdef STM32L4XX
#define GPIO_PORT_TYPEDEF      GPIO_TypeDef
#define GPION_PIN_TYPEDEF      uint16_t
#define ADC_CONTROLLER_TYPEDEF ADC_HandleTypeDef
#define ADC_CHANNEL_TYPEDEF    uint32_t
#define TIMER_HANDLE_TYPEDEF   TIM_HandleTypeDef
#endif

#endif