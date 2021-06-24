/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef _VT_DEFS_H
#define _VT_DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define VT_SUCCESS 0x00
#define VT_ERROR   0x01

#define VT_SIGNATURE_MATCHING                          0x00
#define VT_SIGNATURE_NOT_MATCHING                      0x01
#define VT_SIGNATURE_COMPUTE_FAIL                      0x02
#define VT_SIGNATURE_DB_EMPTY                          0x03
#define VT_SIGNATURE_EVAL_DEFAULT_VALUE                0x04
#define VT_SIGNATURE_DRIFT_DEFAULT_VALUE               0
#define VT_SIGNATURE_TEMPLATE_CONFIDENCE_DEFAULT_VALUE 0
#define VT_DATA_NOT_AVAILABLE                          0xFF

#define VT_CHARACTERS_IN_A_NUMBER              10
#define VT_COMPONENT_NAME_MAX_LENGTH           30
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

#endif