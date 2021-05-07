/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_fc_api.h"

VT_VOID vt_fallcurve_object_initialize(VT_FALLCURVE_OBJECT* fc_object, VT_GPIO_ID gpio_id, VT_GPIO_PORT* gpio_port,
                                          VT_GPIO_PIN* gpio_pin, VT_ADC_ID adc_id, VT_ADC_CONTROLLER* adc_controller,
                                          VT_ADC_CHANNEL* adc_channel, VT_DEVICE_DRIVER* device_driver)
{
   fc_object->sensor.gpio_id = gpio_id;
   fc_object->sensor.gpio_port = gpio_port;
   fc_object->sensor.gpio_pin = gpio_pin;
   fc_object->sensor.adc_id = adc_id;
   fc_object->sensor.adc_controller = adc_controller;
   fc_object->sensor.adc_channel = adc_channel;

   fc_object->device_driver = device_driver;

   fc_reset_db(fc_object);
}