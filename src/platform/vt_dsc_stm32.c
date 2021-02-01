/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_dsc.h"

uint32_t _vt_dsc_flash_get_page(uint32_t Address);
uint32_t _vt_dsc_flash_get_bank(uint32_t Address);

uint32_t _vt_dsc_delay_msec(uint32_t delay)
{
    HAL_Delay(delay);

    return VT_SUCCESS;
}

uint32_t _vt_dsc_delay_usec(TIMER_HANDLE_TYPEDEF* timer, uint32_t delay)
{
    uint32_t start_time;

    HAL_TIM_Base_Start(timer);
    start_time = __HAL_TIM_GET_COUNTER(timer);
    while (__HAL_TIM_GET_COUNTER(timer) - start_time < delay)
        ;
    HAL_TIM_Base_Stop(timer);

    return VT_SUCCESS;
}

uint32_t _vt_dsc_gpio_read(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, int* state)
{
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET)
    {
        *state = 1;
    }

    else if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
    {
        *state = 0;
    }

    return VT_SUCCESS;
}

uint32_t _vt_dsc_gpio_turn_on(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);

    return VT_SUCCESS;
}

uint32_t _vt_dsc_gpio_turn_off(GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);

    return VT_SUCCESS;
}

uint32_t _vt_dsc_adc_read(ADC_CONTROLLER_TYPEDEF* ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel, uint32_t* value)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel      = ADC_Channel;
    sConfig.Rank         = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    HAL_ADC_ConfigChannel(ADC_Controller, &sConfig);

    HAL_ADC_Start(ADC_Controller);
    if (HAL_ADC_PollForConversion(ADC_Controller, 10) == HAL_OK)
    {
        *value = (HAL_ADC_GetValue(ADC_Controller));
    }

    return VT_SUCCESS;
}

uint32_t _vt_dsc_flash_erase(uint32_t flashAddress, uint32_t Nsize)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;

    uint32_t startpage = _vt_dsc_flash_get_page(flashAddress);
    if (startpage == VT_FLASH_ADDRESS_ERROR)
    {
        return VT_FLASH_ADDRESS_ERROR;
    }

    uint32_t endpage = _vt_dsc_flash_get_page(flashAddress + Nsize * 8);
    if (endpage == VT_FLASH_ADDRESS_ERROR)
    {
        return VT_FLASH_WRITE_SIZE_ERROR;
    }

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks     = _vt_dsc_flash_get_bank(flashAddress);
    EraseInitStruct.Page      = startpage;
    EraseInitStruct.NbPages   = startpage - endpage + 1;

    HAL_FLASH_Unlock();

    HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

    HAL_FLASH_Lock();

    return VT_SUCCESS;
}

uint32_t _vt_dsc_flash_write(uint32_t flashAddress, void* wrBuf, uint32_t Nsize)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;

    uint32_t startpage = _vt_dsc_flash_get_page(flashAddress);
    if (startpage == VT_FLASH_ADDRESS_ERROR)
    {
        return VT_FLASH_ADDRESS_ERROR;
    }

    uint32_t endpage = _vt_dsc_flash_get_page(flashAddress + Nsize * 8);
    if (endpage == VT_FLASH_ADDRESS_ERROR)
    {
        return VT_FLASH_WRITE_SIZE_ERROR;
    }

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks     = _vt_dsc_flash_get_bank(flashAddress);
    EraseInitStruct.Page      = startpage;
    EraseInitStruct.NbPages   = endpage - startpage + 1;

    printf("\n\n Bank = %d, Start = %d, End = %d", (int)EraseInitStruct.Banks, (int)startpage, (int)endpage);

    // Unlock Flash
    HAL_FLASH_Unlock();

    HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

    for (uint32_t i = 0; i < Nsize; i++, flashAddress += 8)
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flashAddress, ((uint64_t*)wrBuf)[i]);

    // Lock the Flash space
    HAL_FLASH_Lock();

    return VT_SUCCESS;
}

uint32_t _vt_dsc_flash_read(uint32_t flashAddress, void* rdBuf, uint32_t Nsize)
{
    for (uint32_t i = 0; i < Nsize; i++, flashAddress += 8)
        *((uint64_t*)rdBuf + i) = *(uint64_t*)flashAddress;

    return VT_SUCCESS;
}

/*

Bank 1
            0x0800 0000 - 0x0800 07FF 2 K Page 0
            0x0800 0800 - 0x0800 0FFF 2 K Page 1
            0x0800 1000 - 0x0800 17FF 2 K Page 2
            0x0800 1800 - 0x0800 1FFF 2 K Page 3
            -
            -

            0x0807 F800 - 0x0807 FFFF 2 K Page 255


Bank 2
            0x0808 0000 - 0x0808 07FF 2 K Page 256
            0x0808 0800 - 0x0808 0FFF 2 K Page 257
            0x0808 1000 - 0x0808 17FF 2 K Page 258
            0x0808 1800 - 0x0808 1FFF 2 K Page 259
            -
            -
            0x080F F800 - 0x080F FFFF 2 K Page 511

*/

uint32_t _vt_dsc_flash_get_bank(uint32_t Address)
{
    if ((Address >= 0x08000000) && (Address < 0x0807FFFF))
    {
        return FLASH_BANK_1;
    }

    else if ((Address >= 0x08080000) && (Address < 0x080FFFFF))
    {
        return FLASH_BANK_2;
    }

    return VT_ERROR;
}

uint32_t _vt_dsc_flash_get_page(uint32_t Address)
{
    uint32_t startaddress = 0x08000000;
    if (Address >= 0x08080000)
    {
        startaddress = 0x08080000;
    }

    for (int indx = 0; indx <= 255; indx++)
    {
        if ((Address >= (startaddress + 2048 * indx)) && (Address < (startaddress + (2048 * (indx + 1)))))
        {
            return indx;
        }
    }

    return VT_FLASH_ADDRESS_ERROR;
}
