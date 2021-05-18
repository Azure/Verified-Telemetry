/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file vt_defs.h */

#ifndef _VT_DEFS_H
#define _VT_DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define abs_custom(x) ((x) > 0 ? (x) : -(x))

#define VT_SUCCESS 0X00
#define VT_ERROR   0X01

#define VT_SIGNATURE_MATCHING     0X00
#define VT_SIGNATURE_NOT_MATCHING 0X01
#define VT_SIGNATURE_COMPUTE_FAIL 0X02
#define VT_SIGNATURE_DB_EMPTY     0X03

#define VT_UINT  uint16_t
#define VT_INT   int16_t
#define VT_UINT8 uint8_t
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
