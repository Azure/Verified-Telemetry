/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef _VT_DEVICE_DRIVER_H
#define _VT_DEVICE_DRIVER_H

#include <stdint.h>

/**
 * @brief Initializes ADC controller and channel for single adc read
 *
 * @param[in] adc_id User defined ADC controller/Channel identifier for a particular sensor / current measurement circuit.
 * Variables adc_controller and adc_channel can be used alternatively.
 * @param[in] adc_controller Void pointer to the system ADC Controller to which a particular sensor / current measurement circuit
 * is connected.
 * @param[in] adc_channel Void pointer to the system ADC Channel to which a particular sensor / current measurement circuit is
 * connected.
 * @param[in] adc_resolution Pointer to a value representing ADC resolution. Value would be updated by function based on the
 * resolution of the ADC being used.
 * @param[in] adc_ref_volt Pointer to a value representing ADC reference voltage. Value would be updated by function based on the
 * reference voltage of the ADC being used.
 *
 * @retval 0x00 upon success or 0x01 upon failure.
 */
uint16_t vt_adc_single_read_init(
    uint16_t adc_id, void* adc_controller, void* adc_channel, uint16_t* adc_resolution, float* adc_ref_volt);

/**
 * @brief Returns one datapoint of voltage data converted by the initialized ADC
 *
 * @param[in] adc_id User defined ADC controller/Channel identifier for a particular sensor / current measurement circuit.
 * Variables adc_controller and adc_channel can be used alternatively.
 * @param[in] adc_controller Void pointer to the system ADC Controller to which a particular sensor / current measurement circuit
 * is connected.
 * @param[in] adc_channel Void pointer to the system ADC Channel to which a particular sensor / current measurement circuit is
 * connected.
 *
 * @retval 0x00 upon success or 0x01 upon failure.
 */
uint16_t vt_adc_single_read(uint16_t adc_id, void* adc_controller, void* adc_channel);

/**
 * @brief Reads N voltage datapoints using DMA at a particular sampling frequency
 *
 * @param[in] adc_id User defined ADC controller/Channel identifier for a particular sensor / current measurement circuit.
 * Variables adc_controller and adc_channel can be used alternatively.
 * @param[in] adc_controller Void pointer to the system ADC Controller to which a particular sensor / current measurement circuit
 * is connected.
 * @param[in] adc_channel Void pointer to the system ADC Channel to which a particular sensor / current measurement circuit is
 * connected.
 * @param[in] adc_read_buffer Buffer in which datapoints would be stored.
 * @param[in] buffer_length Length of buffer in which datapoints would be stored / Number of datapoints to collect.
 * @param[in] desired_sampling_frequency Sampling frequency at which data from ADC should be ideally sampled.
 * @param[in] set_sampling_frequency Pointer to a variable which would store sampling frequency at which data from ADC would be sampled.
 * @param[in] vt_adc_buffer_read_conv_half_cplt_callback Pointer to a function that would be called when half of the buffer is
 * filled with datapoints.
 * @param[in] vt_adc_buffer_read_conv_cplt_callback Pointer to a function that would be called when the buffer is filled with
 * datapoints.
 
 * 
 */
void vt_adc_buffer_read(uint16_t adc_id,
    void* adc_controller,
    void* adc_channel,
    float* adc_read_buffer,
    uint16_t buffer_length,
    float desired_sampling_frequency,
    float* set_sampling_frequency,
    void (*vt_adc_buffer_read_conv_half_cplt_callback)(),
    uint16_t (*vt_adc_buffer_read_conv_cplt_callback)(uint16_t mode));

/**
 * @brief Set a GPIO Pin to HIGH
 *
 * @param[in] gpio_id User defined GPIO port and pin identifier which controls a particular analog sensor. Variables gpio_port and
 * gpio_pin can be used alternatively.
 * @param[in] gpio_port Void pointer to the system GPIO Controller which controls a particular analog sensor.
 * @param[in] gpio_pin Void pointer to the system GPIO Pin which controls a particular analog sensor.
 *
 * @retval 0x00 upon success or 0x01 upon failure.
 */
uint16_t vt_gpio_on(uint16_t gpio_id, void* gpio_port, void* gpio_pin);

/**
 * @brief Reset a GPIO Pin to LOW
 *
 * @param[in] gpio_id User defined GPIO port and pin identifier which controls a particular analog sensor. Variables gpio_port and
 * gpio_pin can be used alternatively.
 * @param[in] gpio_port Void pointer to the system GPIO Controller which controls a particular analog sensor.
 * @param[in] gpio_pin Void pointer to the system GPIO Pin which controls a particular analog sensor.
 *
 * @retval 0x00 upon success or 0x01 upon failure.
 */
uint16_t vt_gpio_off(uint16_t gpio_id, void* gpio_port, void* gpio_pin);

/**
 * @brief Initializes and starts custom tick counter using system timer
 *
 * @param[in] max_value Pointer to a value representing max value that the tick counter can reach. Value would be updated by
 * function if it is null.
 * @param[in] resolution_usec Pointer to a value representing resolution of the the tick counter in microseconds. Value would be
 * updated by function if it is null. If value is non-zero, function would use this value to set timer PRESCALER.
 *
 * @retval 0x00 upon success or 0x01 upon failure.
 */
uint16_t vt_tick_init(uint16_t* max_value, uint16_t* resolution_usec);

/**
 * @brief De-initializes and stops custom tick counter
 *
 * @retval 0x00 upon success or 0x01 upon failure.
 */
unsigned long vt_tick_deinit();

/**
 * @brief Returns the current tick count of a initalized custom tick counter using system timer
 *
 * @retval Current tick count
 */
unsigned long vt_tick();

/**
 * @brief Enable global interrupts
 */
void vt_interrupt_enable();

/**
 * @brief Disable global interrupts
 */
void vt_interrupt_disable();

#endif // _VT_DEVICE_DRIVER_TEMPLATE_H
