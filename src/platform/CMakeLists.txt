# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project(vt_platform LANGUAGES C)
set(CMAKE_C_STANDARD 99)

if(VT_DEVICE STREQUAL "MXCHIP_AZ3166")

  add_library(vt_platform STATIC
    ${CMAKE_CURRENT_LIST_DIR}/vt_dsc_mxchip.c
  )

  target_link_libraries(vt_platform
    PRIVATE
      stm32cubef4
  )

  target_compile_definitions(vt_platform PUBLIC "STM32F4XX")

elseif(VT_DEVICE STREQUAL "STM_BL475EIOT01A")

  add_library(vt_platform STATIC
      ${CMAKE_CURRENT_LIST_DIR}/vt_dsc_stm32.c
  )

  target_link_libraries(vt_platform
    PRIVATE
      stm32cubel4
  )

    target_compile_definitions(vt_platform PUBLIC "STM32L4XX")

else()
  #CUSTOM_DEVICE

  add_library(vt_platform  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/vt_dsc_custom.c
  )

  message(WARNING  "<CUSTOM_DEVICE> has been selected as device type, kindly make sure all functions in <src/platform/vt_dsc_custom.c> are implemented to avoid any runtime errors.")
endif()

target_include_directories (vt_platform
  PUBLIC
  $<BUILD_INTERFACE:${vt_SOURCE_DIR}/inc/platform>
  $<BUILD_INTERFACE:${vt_SOURCE_DIR}/inc/core>
)