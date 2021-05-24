# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET vt_core_test)

add_executable(${TARGET} 
    main.c
    test_vt_fc_object_sensor.c
    test_vt_fc_object_database.c
    test_vt_fc_object_initialize.c
)

target_link_libraries(${TARGET}
    PRIVATE
        verified_telemetry
        cmocka-static
)

add_test(
    NAME ${TARGET} 
    COMMAND ${TARGET}
)
