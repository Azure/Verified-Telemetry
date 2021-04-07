# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

project(vt_core_test LANGUAGES C)

add_executable(vt_core_test 
    main.c
    test_vt_fingerprint.c
    test_vt_sensor.c
    test_vt_database.c
)

target_link_libraries(vt_core_test
    PRIVATE
        verified_telemetry
        cmocka-static
)

add_test(NAME vt_core_test COMMAND vt_core_test)
