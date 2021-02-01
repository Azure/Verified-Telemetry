/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_PORT_H
#define _VT_PORT_H

#ifdef MXCHIP_AZ3166
#include "stm32f4xx_hal.h"
#endif
#ifdef STM_BL475EIOT01A
#include "stm32l4xx_hal.h"
#endif

#ifdef MXCHIP_AZ3166
#define GPIO_PORT_TYPEDEF		GPIO_TypeDef
#define GPION_PIN_TYPEDEF		uint16_t
#define ADC_CONTROLLER_TYPEDEF	ADC_HandleTypeDef
#define ADC_CHANNEL_TYPEDEF		uint16_t
#define TIMER_HANDLE_TYPEDEF    TIM_HandleTypeDef
#endif

#ifdef STM_BL475EIOT01A
#define GPIO_PORT_TYPEDEF		GPIO_TypeDef
#define GPION_PIN_TYPEDEF		uint16_t
#define ADC_CONTROLLER_TYPEDEF	ADC_HandleTypeDef
#define ADC_CHANNEL_TYPEDEF		long
#define TIMER_HANDLE_TYPEDEF    TIM_HandleTypeDef
#endif



#endif