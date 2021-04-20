/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_DATABASE_H
#define _VT_DATABASE_H

#include <stdint.h>

#include "vt_api.h"

// Initialize
void _vt_database_initialize_fingerprintdb(VT_DATABASE* database_ptr);

void _vt_database_initialize_falltimedb(VT_DATABASE* database_ptr);

void _vt_database_initialize_pearsoncoefficientdb(VT_DATABASE* database_ptr);

// Store
void _vt_database_store_falltime(VT_DATABASE* database_ptr, uint32_t fall_time, uint8_t sensor_id);

void _vt_database_store_pearsoncoefficient(VT_DATABASE* database_ptr, float pearson_coefficient, uint8_t sensor_id);

// Check if Empty
uint32_t _vt_database_check_pearson_falltime_availability(VT_DATABASE* database_ptr);

// Evaluate
int8_t _vt_database_evaluate_nrmse(VT_DATABASE* database_ptr, uint32_t* fallcurvearray);

int8_t _vt_database_evaluate_pearson_falltime(VT_DATABASE* database_ptr, uint32_t fall_time, float pearson_coefficient);

#endif
