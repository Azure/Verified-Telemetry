/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_device_driver.h"

#define SAMPLE_INTERNAL_ADC_TYPE_ID  0x01
#define SAMPLE_INTERNAL_GPIO_TYPE_ID 0x01

void (*vt_adc_buffer_read_conv_half_cplt_callback_stored)();
void (*vt_adc_buffer_read_conv_cplt_callback_stored)();

uint16_t vt_adc_single_read_init(
    uint16_t adc_id, void* adc_controller, void* adc_channel, uint16_t* adc_resolution, float* adc_ref_volt)
{
    if (adc_id == SAMPLE_INTERNAL_ADC_TYPE_ID)
    {
        /* Configure Internal ADC Controller and Channel */

        /* Set Internal ADC resolution and reference voltage */
        *adc_resolution = 12;
        *adc_ref_volt   = 3.3f;
    }
    return 0;
}

uint16_t vt_adc_single_read(uint16_t adc_id, void* adc_controller, void* adc_channel)
{
    uint16_t value = 0;

    if (adc_id == SAMPLE_INTERNAL_ADC_TYPE_ID)
    {
        /* Update variable value with the value read from the particular ADC controller and channel */
    }
    return value;
}

uint16_t vt_adc_buffer_read(uint16_t adc_id,
    void* adc_controller,
    void* adc_channel,
    float* adc_read_buffer,
    uint16_t buffer_length,
    float sampling_frequency,
    void (*vt_adc_buffer_read_conv_half_cplt_callback)(),
    void (*vt_adc_buffer_read_conv_cplt_callback)())
{
    /* Store callbacks in local memory */
    vt_adc_buffer_read_conv_half_cplt_callback_stored = vt_adc_buffer_read_conv_half_cplt_callback;
    vt_adc_buffer_read_conv_cplt_callback_stored      = vt_adc_buffer_read_conv_cplt_callback;

    /* Start reading buffer_length datapoints from the particular adc_channel of adc_controller at sampling_frequency using DMA
     * which stores data in adc_read_buffer */
    /* ADC_ConvHalfCpltCallback would be called by system when adc_read_buffer is half filled and ADC_ConvCpltCallback would be
     * called when adc_read_buffer is completely filled */
}

// Called by system when first half of buffer is filled
static void ADC_ConvHalfCpltCallback()
{
    vt_adc_buffer_read_conv_half_cplt_callback_stored();
}

// Called by system when buffer is completely filled
static void ADC_ConvCpltCallback()
{
    vt_adc_buffer_read_conv_cplt_callback_stored();
}

uint16_t vt_gpio_on(uint16_t gpio_id, void* gpio_port, void* gpio_pin)
{
    if (gpio_id == SAMPLE_INTERNAL_GPIO_TYPE_ID)
    {
        /* SET GPIO Pin */
    }
    return 0;
}

uint16_t vt_gpio_off(uint16_t gpio_id, void* gpio_port, void* gpio_pin)
{
    if (gpio_id == SAMPLE_INTERNAL_GPIO_TYPE_ID)
    {
        /* RESET GPIO Pin */
    }
    return 0;
}

uint16_t vt_tick_init(uint16_t* max_value, uint16_t* resolution_usec)
{
    uint16_t default_max_tick        = 65535;
    uint16_t default_tick_resolution = 1;
    if (*max_value)
    {
        default_max_tick = *max_value;
    }
    else
    {
        *max_value = default_max_tick;
    }
    if (*resolution_usec)
    {
        default_tick_resolution = *resolution_usec;
    }
    else
    {
        *resolution_usec = default_tick_resolution;
    }

    /* Initialize Timer */
    /* Start Timer */

    return 0;
}

unsigned long vt_tick_deinit()
{
    /* De-Initialize Timer */
    /* Stop Timer */
    return 0;
}

unsigned long vt_tick()
{
    /* Return Timer value */
}

void vt_interrupt_enable()
{
    /* Enable global interrupts */
}

void vt_interrupt_disable()
{
    /* Disable global interrupts */
}
