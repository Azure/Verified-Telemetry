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
        -Wl,--wrap=_vt_dsc_delay_usec
        -Wl,--wrap=_vt_dsc_gpio_turn_on
        -Wl,--wrap=_vt_dsc_gpio_turn_off
        -Wl,--wrap=_vt_dsc_adc_read
)

# setup code coverage
append_coverage_compiler_flags()
setup_target_for_coverage_lcov(
    NAME ${PROJECT_NAME}_coverage
    EXECUTABLE ${PROJECT_NAME}
#    EXCLUDE ${COV_EXCLUDE}
#    LCOV_ARGS --rc lcov_branch_coverage=1
)

add_test(NAME ${PROJECT_NAME} COMMAND vt_core_test)
