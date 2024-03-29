# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET vt_core_test)

add_executable(${TARGET} 
    main.c
    fallcurve/test_vt_fc_object_sensor.c
    fallcurve/test_vt_fc_object_database.c
    fallcurve/test_vt_fc_object_initialize.c
    currentsense/test_vt_cs_object_signature.c
    currentsense/test_vt_cs_object_database.c
    currentsense/test_vt_cs_object_initialize.c
    currentsense/test_vt_cs_object_sensor.c
)

target_link_libraries(${TARGET}
    PRIVATE
        az::iot::vt::core
        cmocka-static
)

target_include_directories(${TARGET}
    PRIVATE 
        fallcurve
        currentsense
)

add_test(
    NAME ${TARGET} 
    COMMAND ${TARGET}
)
