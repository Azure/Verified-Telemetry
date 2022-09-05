/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef _VT_DEFS_H
#define _VT_DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define abs_custom(x) ((x) > 0 ? (x) : -(x))

#define VT_SUCCESS 0x00
#define VT_ERROR   0x01

#define VT_CS_BOTH_REPEATING_NONREPEATING_SIGNATURE_VALID 0X00
#define VT_CS_ONLY_REPEATING_SIGNATURE_VALID 0X01
#define VT_CS_ONLY_NONREPEATING_SIGNATURE_VALID 0X02
#define VT_CS_NO_SIGNATURE_VALID 0X03


#define MULTICALIBRATION_COUNT 4
#define MIN_MULTICALIBRATION_MATCHING_COUNT 3

#define VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD 20
#define VT_CS_REP_SIGNATURE_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD 10
#define VT_CS_REP_DUTY_CYCLE_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD 40
#define VT_CS_REP_RELATIVE_CURR_DRAW_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD 40

#define VT_CS_NONREP_AVG_ON_CURR_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD 30
#define VT_CS_NONREP_AVG_OFF_CURR_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD 30

#define VT_HISTORY_ARRAY_LENGTH 3

#define VT_SIGNATURE_MATCHING     0x00
#define VT_SIGNATURE_NOT_MATCHING 0x01
#define VT_SIGNATURE_COMPUTE_FAIL 0x02
#define VT_SIGNATURE_DB_EMPTY     0x03
#define VT_SIGNATURE_EVAL_DEFAULT_VALUE                0x04
#define VT_SIGNATURE_DRIFT_DEFAULT_VALUE               0
#define VT_SIGNATURE_TEMPLATE_CONFIDENCE_DEFAULT_VALUE 0
#define VT_DATA_NOT_AVAILABLE                          -1

#define VT_CHARACTERS_IN_A_NUMBER              10
#define VT_COMPONENT_NAME_MAX_LENGTH           50
#define VT_ASSOCIATED_TELEMETRY_CSV_MAX_LENGTH 100

#define VT_MODE_RUNTIME_EVALUATE 0x00
#define VT_MODE_CALIBRATE        0x01
#define VT_MODE_RECALIBRATE      0x02

#define VT_DB_NOT_UPDATED 0x00
#define VT_DB_UPDATED     0x01

#define VT_UINT  uint16_t
#define VT_INT   int16_t
#define VT_UINT8 uint8_t
#define VT_INT32 int32_t
#define VT_ULONG unsigned long
#define VT_UCHAR unsigned char
#define VT_CHAR  char
#define VT_VOID  void
#define VT_FLOAT float
#define VT_BOOL  bool

#define VT_ADC_ID         VT_UINT
#define VT_ADC_CONTROLLER VT_VOID
#define VT_ADC_CHANNEL    VT_VOID
#define VT_GPIO_ID        VT_UINT
#define VT_GPIO_PORT      VT_VOID
#define VT_GPIO_PIN       VT_VOID

//defines
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;

#endif
