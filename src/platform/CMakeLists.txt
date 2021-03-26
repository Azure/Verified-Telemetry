# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.8)

project(vt_platform LANGUAGES C)
set(CMAKE_C_STANDARD 99)

if(VT_DEVICE STREQUAL "MXCHIP_AZ3166")
  add_library(vt_mxchip STATIC
    ${CMAKE_CURRENT_LIST_DIR}/vt_dsc_mxchip.c
  )

  target_link_libraries(vt_platform
    PRIVATE
      stm32cubef4
  )
elseif(VT_DEVICE STREQUAL "STM_BL475EIOT01A")

  add_library(vt_stm32 STATIC
      ${CMAKE_CURRENT_LIST_DIR}/vt_dsc_stm32.c
  )

  target_link_libraries(vt_platform
    PRIVATE
      stm32cubel4
  )
else()
  #CUSTOM_DEVICE

  add_library(vt_custom  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/vt_dsc_custom.c
  )

  target_link_libraries(vt_custom
    PRIVATE
      vt_core
  )

  message(WARNING  "<CUSTOM_DEVICE> has been selected as device type, kindly make sure all functions in <src/platform/vt_dsc_custom.c> are implemented to avoid any runtime errors.")
endif()