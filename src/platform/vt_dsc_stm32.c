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

        HAL_TIM_Base_Start(timer);
        start_time = __HAL_TIM_GET_COUNTER(timer);
        while (__HAL_TIM_GET_COUNTER(timer) - start_time < delay)
            ;
        HAL_TIM_Base_Stop(timer);
    }

    return VT_PLATFORM_SUCCESS;
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
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset       = 0;

    HAL_ADC_ConfigChannel(adc_controller, &sConfig);

    HAL_ADC_Start(adc_controller);
    if (HAL_ADC_PollForConversion(adc_controller, 10) == HAL_OK)
    {
        *value = (HAL_ADC_GetValue(adc_controller));
    }
    HAL_ADC_Stop(adc_controller);

    return VT_PLATFORM_SUCCESS;
}