/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_dsc.h"

uint32_t _vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay)
{
    if (timer == NULL)
    {
        if (delay > 1000)
        {
            uint32_t msec_delay = delay / 1000;
            HAL_Delay(msec_delay);
        }
        else
        {
            HAL_Delay(1);
        }
    }

    else
    {
        uint32_t start_time;
        HAL_TIM_Base_Init(timer);
        HAL_TIM_Base_Start(timer);
        start_time = __HAL_TIM_GET_COUNTER(timer);
        while (__HAL_TIM_GET_COUNTER(timer) - start_time < delay)
            ;
        HAL_TIM_Base_Stop(timer);
    }

    return VT_PLATFORM_SUCCESS;
}

uint32_t _vt_dsc_tick_init(TIMER_HANDLE_TYPEDEF* timer, uint16_t* max_value, uint16_t* resolution_usec)
{
    uint16_t default_max_tick = 65535;
    uint16_t default_tick_resolution = 1;
    if(*max_value)
    {
        default_max_tick = *max_value;
    }
    else
    {
        *max_value = default_max_tick;
    }
    if(*resolution_usec)
    {
        default_tick_resolution = *resolution_usec;
    }
    else
    {
        *resolution_usec = default_tick_resolution;
    }

    __disable_irq();
    if (HAL_TIM_Base_Init((TIM_HandleTypeDef *)timer) != HAL_OK) {
        // add error handling
    }
    HAL_TIM_Base_Start((TIM_HandleTypeDef *)timer);
    return 0; 
}

uint32_t _vt_dsc_tick_deinit(TIMER_HANDLE_TYPEDEF* timer)
{
    HAL_TIM_Base_Stop((TIM_HandleTypeDef *)timer);
    __enable_irq();
    return 0;
}

uint32_t _vt_dsc_tick(TIMER_HANDLE_TYPEDEF* timer)
{
    return __HAL_TIM_GET_COUNTER((TIM_HandleTypeDef *)timer);
}

uint32_t _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin)
{
    HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);

    return VT_PLATFORM_SUCCESS;
}

uint32_t _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* gpio_port, GPIO_PIN_TYPEDEF gpio_pin)
{
    HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_RESET);

    return VT_PLATFORM_SUCCESS;
}

uint32_t _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* adc_controller, ADC_CHANNEL_TYPEDEF adc_channel, uint32_t* value)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel      = adc_channel;
    sConfig.Rank         = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

    HAL_ADC_ConfigChannel(adc_controller, &sConfig);

    HAL_ADC_Start(adc_controller);
    if (HAL_ADC_PollForConversion(adc_controller, 10) == HAL_OK)
    {
        *value = (HAL_ADC_GetValue(adc_controller));
    }
    HAL_ADC_Stop(adc_controller);

    return VT_PLATFORM_SUCCESS;
}
