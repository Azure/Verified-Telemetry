/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef _VT_FC_CONFIG_H
#define _VT_FC_CONFIG_H

#define VT_FC_MIN_FALLTIME_DATAPOINTS    2
#define VT_FC_MIN_SHAPE_MATCH            0.85f
#define VT_FC_MAX_SAMPLING_INTERVAL_US   60000
#define VT_FC_MIN_SAMPLING_INTERVAL_US   1
#define VT_FC_MAX_FALLTIME_DEVIATION     20.0f
#define VT_FC_MAX_SHAPE_DEVIATION_FACTOR 20.0f
#define VT_FC_MAX_SHAPE_DEVIATION        ((1.0f - VT_FC_MIN_SHAPE_MATCH) / VT_FC_MIN_SHAPE_MATCH) * VT_FC_MAX_SHAPE_DEVIATION_FACTOR
#define VT_FC_SAMPLE_LENGTH              100
#define VT_FC_MAX_SIGNATURES             5

#endif
