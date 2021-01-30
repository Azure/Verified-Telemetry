/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _VT_DATABASE_H
#define _VT_DATABASE_H

#include "vt_api.h"
#include "vt_fingerprint.h"

#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

//Initialize
void _vt_database_initialize_fingerpintdb(VT_DATABASE* database_ptr);
void _vt_database_initialize_falltimedb(VT_DATABASE* database_ptr);
void _vt_database_initialize_pearsoncoefficientdb(VT_DATABASE* database_ptr);

// Store
uint32_t  _vt_database_store_fingerprint(VT_DATABASE* database_ptr, uint32_t* fallcurvearray,int sampling_frequency, int sensorid);
uint32_t  _vt_database_store_falltime(VT_DATABASE* database_ptr, int fall_time, int sensorid);
uint32_t  _vt_database_store_pearsoncoefficient(VT_DATABASE* database_ptr, double pearson_coefficient, int sensorid);

//Evaluate
int _vt_database_evaluate_nrmse(VT_DATABASE* database_ptr, uint32_t* fallcurvearray);
int _vt_database_evaluate_pearson_falltime(VT_DATABASE* database_ptr, int fall_time, double pearson_coefficient);

#endif