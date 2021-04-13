# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET vt_core_test)

add_executable(${TARGET} 
    main.c
    test_vt_fingerprint.c
    test_vt_sensor.c
    test_vt_database.c
)

target_link_libraries(${TARGET}
    PRIVATE
        verified_telemetry
        cmocka-static
        -Wl,--wrap=_vt_dsc_adc_read
        -Wl,--wrap=_vt_dsc_delay_usec
        -Wl,--wrap=_vt_dsc_gpio_turn_on
        -Wl,--wrap=_vt_dsc_gpio_turn_off
)

add_test(
    NAME ${TARGET} 
    COMMAND ${TARGET}
)
