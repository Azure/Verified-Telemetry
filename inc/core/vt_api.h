/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_API_H
#define _VT_API_H

#include "stdio.h"

#include "vt_user.h"
#include "vt_port.h"
#include "vt_dsc.h"


#define FALLCURVE_LENGTH 100

#define VT_SUCCESS                              0X00
#define VT_ERROR                                0X01
#define VT_PTR_ERROR                            0X02
#define VT_MUTEX_ERROR                          0X03
#define VT_CUSTOM_IMPLEMENTATION_NOT_PROVIDED   0X04
#define VT_UNIDENTIFIED_SENSOR_NAME             0X05
#define VT_THREAD_SLEEP_ERROR                   0X06
#define VT_REPEATABILITY_ERROR                  0X11
#define VT_RISING_FUNCTION_ERROR                0X12
#define VT_STEP_FUNCTION_ERROR                  0X13
#define VT_NOISY_FUNCTION_ERROR                 0X14

#define FLASH_DB_START_VALUE 0x42


typedef struct VT_SENSOR_STRUCT
{
    char* vt_sensor_name;
    GPIO_PORT_TYPEDEF* vt_gpio_port;
    GPION_PIN_TYPEDEF vt_gpio_pin;

    ADC_CONTROLLER_TYPEDEF* vt_adc_controller;
    ADC_CHANNEL_TYPEDEF vt_adc_channel;

    TIMER_HANDLE_TYPEDEF* vt_timer;

    int vt_sampling_frequency;
} VT_SENSOR;

typedef struct VT_DATABASE_STRUCT
{
    uint32_t vt_flash_address;
    uint32_t vt_fallcurve_component_id;

    int _vt_total_fingerprints;
    uint32_t _vt_fingerprintdb[10][102];

    int _vt_total_falltime;
    int _vt_falltimedb[10][2];

    int _vt_total_pearson_coefficient;
    double _vt_pearson_coefficientdb[10][2];

} VT_DATABASE;

// Initialize
uint32_t  vt_sensor_initialize(VT_SENSOR* sensor_ptr, char* Port_Name, GPIO_PORT_TYPEDEF* GPIOx, GPION_PIN_TYPEDEF GPIO_Pin, ADC_CONTROLLER_TYPEDEF* ADC_Controller, ADC_CHANNEL_TYPEDEF ADC_Channel, TIMER_HANDLE_TYPEDEF* Timer_Handler);

// Calibrate
void vt_sensor_calibrate(VT_SENSOR* sensor_ptr);

// Read
uint32_t  vt_sensor_read_value(VT_SENSOR* sensor_ptr, uint32_t* sensor_value);
uint32_t  vt_sensor_read_fingerprint(VT_SENSOR* sensor_ptr, uint32_t* fingerprint_array, char* fingerprint_string);
uint32_t  vt_sensor_read_status(VT_SENSOR* sensor_ptr, VT_DATABASE* database_ptr, uint32_t* fingerprint, int* sensorid);

// database
uint32_t  vt_database_initialize(VT_DATABASE* database_ptr, uint32_t flash_address, uint32_t fallcurve_component_id);
uint32_t  vt_database_store(VT_DATABASE* database_ptr, uint32_t* fallcurvearray, int sampling_frequency, int sensorid);
uint32_t  vt_database_clear(VT_DATABASE* database_ptr);

// Database Fetch
uint32_t  vt_database_fingerprint_fetch(VT_DATABASE* database_ptr, int* index, uint32_t* fallcurvearray, int* sensorid);
uint32_t  vt_database_falltime_fetch(VT_DATABASE* database_ptr, int* index, int* fall_time, int* sensorid);
uint32_t  vt_database_pearsoncoefficient_fetch(VT_DATABASE* database_ptr, int* index, double* pearson_coefficient, int* sensorid);





#endif
