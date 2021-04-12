# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(PROJECT_NAME vt_core_test)

project(${PROJECT_NAME} LANGUAGES C)

add_executable(${PROJECT_NAME} 
    main.c
    test_vt_fingerprint.c
    test_vt_sensor.c
    test_vt_database.c
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        verified_telemetry
        cmocka-static
        -Wl,--wrap=_vt_dsc_adc_read
        -Wl,--wrap=_vt_dsc_delay_usec
        -Wl,--wrap=_vt_dsc_gpio_turn_on
        -Wl,--wrap=_vt_dsc_gpio_turn_off
)

add_test(
    NAME ${PROJECT_NAME} 
    COMMAND PROJECT_NAME
)

add_code_coverage_target(PROJECT_NAME)
