/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include <math.h>
#include <stdbool.h>
#include "vt_cs_read.h"
#include "vt_debug.h"

#define VOLT_TO_MILLIVOLT 1000.0f
#define SEC_TO_USEC 1000000.0f
#define MAX_LEGAL_TICK_COUNT 65535

static VT_VOID cs_calculate_required_tick_resolution(VT_FLOAT sampling_frequency, VT_UINT* tick_resolution_usec, VT_UINT max_tick_value)
{
   VT_FLOAT sampling_period_ticks;
   if(sampling_frequency == 0)
   {
      return;
   }
   while(true)
   {
      sampling_period_ticks = ((SEC_TO_USEC*1.0f)/sampling_frequency)/((VT_FLOAT)*tick_resolution_usec);
      if(sampling_period_ticks < (VT_FLOAT)max_tick_value)
      {
         break;
      }
      *tick_resolution_usec = *tick_resolution_usec + 1;
   }
}

VT_VOID cs_adc_read(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT *raw_signature, VT_FLOAT sampling_frequency, VT_UINT sample_length)
{
   VT_UINT max_tick_value = 0;
   VT_UINT tick_resolution_usec = 0;
   VT_UINT adc_resolution = 0;
   VT_FLOAT adc_ref_volt = 0;
   VT_UINT start_tick_count = 0;
   VT_UINT sampling_period_ticks = 0;

   cs_object->device_driver->tick_init(&max_tick_value, &tick_resolution_usec);
   fc_calculate_required_tick_resolution(sampling_frequency, &tick_resolution_usec, max_tick_value);
   VT_ULONG sampling_period_ticks = round(((SEC_TO_USEC*1.0f)/sampling_frequency)/((VT_FLOAT)tick_resolution_usec));
   VTLogDebug("Sampling Period Ticks: %d \r\n", sampling_period_ticks);

   cs_object->device_driver->adc_init(
      cs_object->sensor_handle->adc_id, cs_object->sensor_handle->adc_controller, cs_object->sensor_handle->adc_channel, &adc_resolution, &adc_ref_volt);
   cs_object->device_driver->interrupt_disable();
   cs_object->device_driver->tick_init(&max_tick_value, &tick_resolution_usec);
   start_tick_count = (VT_UINT)cs_object->device_driver->tick();

   for (VT_UINT iter = 0; iter < sample_length; iter++)
   {
      raw_signature[iter] = cs_object->device_driver->adc_read(
         cs_object->sensor_handle->adc_id, cs_object->sensor_handle->adc_controller, cs_object->sensor_handle->adc_channel);
      while((uint16_t)((VT_UINT)cs_object->device_driver->tick() - start_tick_count) < sampling_period_ticks){
      }
      start_tick_count = (VT_UINT)cs_object->device_driver->tick();
   }

   cs_object->device_driver->tick_deinit();
   cs_object->device_driver->interrupt_enable();
   for (VT_UINT iter = 0; iter < sample_length; iter++)
   {
      raw_signature[iter] = (((raw_signature[iter]* adc_ref_volt * VOLT_TO_MILLIVOLT)/(VT_FLOAT)pow(2, (VT_FLOAT)adc_resolution))/(VT_FLOAT)VT_CS_ADC_CURR_GAIN);
   }
}