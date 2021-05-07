/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_FC_H
#define _VT_FC_H

#include "vt_fc_config.h"
#include "vt_fc_api.h"

typedef struct VT_FALLCURVE_TEMPLATE_SIGNATURE_STRUCT
{
   VT_ULONG sampling_interval_us;
   VT_ULONG falltime;
   VT_FLOAT pearson_coeff;
} VT_FALLCURVE_TEMPLATE_SIGNATURE;

typedef struct VT_FALLCURVE_DATABASE_STRUCT
{
   VT_FLOAT num_signatures;
   VT_FALLCURVE_TEMPLATE_SIGNATURE db[VT_FC_MAX_SIGNATURES];
} VT_FALLCURVE_DATABASE;

typedef struct VT_FALLCURVE_SENSOR_STRUCT
{
   VT_GPIO_ID gpio_id;
   VT_GPIO_PORT* gpio_port;
   VT_GPIO_PIN* gpio_pin;
   VT_ADC_ID adc_id;
   VT_ADC_CONTROLLER* adc_controller;
   VT_ADC_CHANNEL* adc_channel;
   VT_ULONG max_sampling_interval_us;
   VT_ULONG min_sampling_interval_us;
} VT_FALLCURVE_SENSOR;

#endif
