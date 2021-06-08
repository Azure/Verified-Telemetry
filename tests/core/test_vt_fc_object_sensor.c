/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "vt_test_definitions.h"

#include "vt_fc_api.h"
#include "vt_fc_config.h"

#include "cmocka.h"

static VT_UINT vt_adc_iter;
static VT_ULONG vt_tick_iter;
static VT_UINT vt_adc_inconsistent_response_iter = 0;

static VT_UINT vt_adc_init(
    VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel, VT_UINT* adc_resolution, float* adc_ref_volt)
{
    vt_adc_iter = 0;
    return 0;
}

static VT_UINT vt_adc_init_inconsistent_voltage_response(
    VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel, VT_UINT* adc_resolution, float* adc_ref_volt)
{
    if (vt_adc_inconsistent_response_iter > 2)
    {
        vt_adc_inconsistent_response_iter = 0;
    }
    vt_adc_inconsistent_response_iter++;
    vt_adc_iter = 0;
    return 0;
}

static VT_UINT vt_adc_read_zero_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    return value;
}

static VT_UINT vt_adc_read_constant_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 5;
    return value;
}

static VT_UINT vt_adc_read_step_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 50;
    if (vt_adc_iter == 0)
    {
        value = 100;
    }
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_step_going_to_zero_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 50;
    if (vt_adc_iter == 0)
    {
        value = 100;
    }
    else if (vt_adc_iter == (VT_FC_SAMPLE_LENGTH - 1))
    {
        value = 0;
    }
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_triangular_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    if (vt_adc_iter < (VT_FC_SAMPLE_LENGTH / 2))
    {
        value = vt_adc_iter + 1;
    }
    else
    {
        value = (2 * (VT_FC_SAMPLE_LENGTH / 2) - vt_adc_iter) - 1;
    }
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_exponential_rise_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    value = round((VT_FLOAT)1000 * (VT_FLOAT)exp(1.0f - (-1.0f * ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)(VT_FC_SAMPLE_LENGTH - 1)))));
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_exponential_fall_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    value         = round((VT_FLOAT)1000 * (VT_FLOAT)exp(-1.0f * ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)(VT_FC_SAMPLE_LENGTH - 1))));
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_fast_exponential_fall_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    value = round((VT_FLOAT)1000 * (VT_FLOAT)exp(-1.0f * ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)((VT_FC_SAMPLE_LENGTH / 3) - 1))));
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_slow_exponential_fall_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    value = round((VT_FLOAT)1000 * (VT_FLOAT)exp(-1.0f * ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)((VT_FC_SAMPLE_LENGTH * 3) - 1))));
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_different_amplitude_exponential_fall_voltage_response(
    VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    value         = round((VT_FLOAT)500 * (VT_FLOAT)exp(-1.0f * ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)(VT_FC_SAMPLE_LENGTH - 1))));
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_half_exponential_half_linear_fall_voltage_response(
    VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    VT_UINT value = 0;
    if (vt_adc_iter < (VT_FC_SAMPLE_LENGTH / 2))
    {
        value = round((VT_FLOAT)1000 * (VT_FLOAT)exp(-1.0f * ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)(VT_FC_SAMPLE_LENGTH - 1))));
    }
    else
    {
        value = round((VT_FLOAT)1000 * (1.0f - ((VT_FLOAT)vt_adc_iter / (VT_FLOAT)VT_FC_SAMPLE_LENGTH)));
    }
    vt_adc_iter++;
    return value;
}

static VT_UINT vt_adc_read_inconsistent_voltage_response(VT_UINT adc_id, VT_VOID* adc_controller, VT_VOID* adc_channel)
{
    if (vt_adc_inconsistent_response_iter < 3)
    {
        return vt_adc_read_exponential_fall_voltage_response(adc_id, adc_controller, adc_channel);
    }
    else
    {
        return vt_adc_read_step_voltage_response(adc_id, adc_controller, adc_channel);
    }
}

static VT_UINT vt_gpio_on(VT_UINT gpio_id, VT_VOID* gpio_port, VT_VOID* gpio_pin)
{
    return 0;
}

static VT_UINT vt_gpio_off(VT_UINT gpio_id, VT_VOID* gpio_port, VT_VOID* gpio_pin)
{
    return 0;
}

static VT_UINT vt_tick_init(VT_UINT* max_value, VT_UINT* resolution_usec)
{
    vt_tick_iter = 0;
    return 0;
}

static VT_UINT vt_tick_init_with_some_values(VT_UINT* max_value, VT_UINT* resolution_usec)
{
    *max_value       = 5;
    *resolution_usec = 5;
    vt_tick_iter     = 0;
    return 0;
}

static unsigned long vt_tick_deinit()
{
    return 0;
}

static unsigned long vt_tick()
{
    vt_tick_iter += 1;
    return vt_tick_iter;
}

void vt_interrupt_enable()
{
    // do nothing
}

void vt_interrupt_disable()
{
    // do nothing
}

// vt_fallcurve_object_sensor_calibrate() & vt_fallcurve_object_sensor_recalibrate()
static VT_VOID test_vt_fallcurve_object_sensor_calibrate_recalibrate(VT_VOID** state)
{
    VT_FALLCURVE_OBJECT fc_object;
    VT_DEVICE_DRIVER device_driver;
    VT_SENSOR_HANDLE sensor_handle;
    fc_object.device_driver = &device_driver;
    fc_object.sensor_handle = &sensor_handle;
    VT_UINT8 confidence_metric;
    fc_object.device_driver->adc_init          = &vt_adc_init;
    fc_object.device_driver->gpio_on           = &vt_gpio_on;
    fc_object.device_driver->gpio_off          = &vt_gpio_off;
    fc_object.device_driver->tick_init         = &vt_tick_init;
    fc_object.device_driver->tick_deinit       = &vt_tick_deinit;
    fc_object.device_driver->tick              = &vt_tick;
    fc_object.device_driver->interrupt_enable  = &vt_interrupt_enable;
    fc_object.device_driver->interrupt_disable = &vt_interrupt_disable;

    fc_object.device_driver->adc_read = &vt_adc_read_zero_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);

    fc_object.device_driver->adc_read = &vt_adc_read_constant_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);

    fc_object.device_driver->adc_read = &vt_adc_read_step_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);

    fc_object.device_driver->adc_read = &vt_adc_read_exponential_rise_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);

    fc_object.device_driver->adc_read = &vt_adc_read_triangular_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 1);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 2);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 3);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 4);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 5);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 5);

    fc_object.device_driver->adc_read = &vt_adc_read_exponential_fall_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 1);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 2);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 3);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 4);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_SUCCESS);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 5);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(fc_object.fingerprintdb.num_signatures, 5);

    fc_object.device_driver->adc_read = &vt_adc_read_fast_exponential_fall_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_SUCCESS);

    fc_object.device_driver->adc_read = &vt_adc_read_slow_exponential_fall_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_SUCCESS);

    fc_object.device_driver->adc_init = &vt_adc_init_inconsistent_voltage_response;
    fc_object.device_driver->adc_read = &vt_adc_read_inconsistent_voltage_response;
    assert_int_equal(vt_fallcurve_object_sensor_calibrate(&fc_object, &confidence_metric), VT_ERROR);
    assert_int_equal(vt_fallcurve_object_sensor_recalibrate(&fc_object, &confidence_metric), VT_ERROR);
}

// vt_fallcurve_object_sensor_status()
static VT_VOID test_vt_fallcurve_object_sensor_status(VT_VOID** state)
{
    VT_FALLCURVE_OBJECT fc_object;
    VT_DEVICE_DRIVER device_driver;
    VT_SENSOR_HANDLE sensor_handle;
    fc_object.device_driver = &device_driver;
    fc_object.sensor_handle = &sensor_handle;
    VT_UINT sensor_status;
    VT_UINT sensor_drift;
    fc_object.device_driver->adc_init          = &vt_adc_init;
    fc_object.device_driver->gpio_on           = &vt_gpio_on;
    fc_object.device_driver->gpio_off          = &vt_gpio_off;
    fc_object.device_driver->tick_init         = &vt_tick_init;
    fc_object.device_driver->tick_deinit       = &vt_tick_deinit;
    fc_object.device_driver->tick              = &vt_tick;
    fc_object.device_driver->interrupt_enable  = &vt_interrupt_enable;
    fc_object.device_driver->interrupt_disable = &vt_interrupt_disable;

    fc_object.device_driver->adc_read      = &vt_adc_read_zero_voltage_response;
    fc_object.fingerprintdb.num_signatures = 0;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_DB_EMPTY);

    fc_object.fingerprintdb.num_signatures = 5;
    for (VT_UINT iter = 0; iter < VT_FC_MAX_SIGNATURES - 2; iter++)
    {
        fc_object.fingerprintdb.db[iter].sampling_interval_us = 200;
        fc_object.fingerprintdb.db[iter].falltime             = 200;
        fc_object.fingerprintdb.db[iter].pearson_coeff        = 0.85f;
    }

    fc_object.fingerprintdb.db[VT_FC_MAX_SIGNATURES - 2].sampling_interval_us = 1;
    fc_object.fingerprintdb.db[VT_FC_MAX_SIGNATURES - 2].falltime             = 100;
    fc_object.fingerprintdb.db[VT_FC_MAX_SIGNATURES - 2].pearson_coeff        = 0.90f;

    fc_object.fingerprintdb.db[VT_FC_MAX_SIGNATURES - 1].sampling_interval_us = 1;
    fc_object.fingerprintdb.db[VT_FC_MAX_SIGNATURES - 1].falltime             = 100;
    fc_object.fingerprintdb.db[VT_FC_MAX_SIGNATURES - 1].pearson_coeff        = 1.0f;

    fc_object.device_driver->adc_read = &vt_adc_read_zero_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_COMPUTE_FAIL);

    fc_object.device_driver->adc_read = &vt_adc_read_constant_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_COMPUTE_FAIL);

    fc_object.device_driver->adc_read = &vt_adc_read_step_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_COMPUTE_FAIL);

    fc_object.device_driver->adc_read = &vt_adc_read_step_going_to_zero_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_COMPUTE_FAIL);

    fc_object.device_driver->adc_read = &vt_adc_read_exponential_rise_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_COMPUTE_FAIL);

    fc_object.device_driver->adc_read = &vt_adc_read_triangular_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_NOT_MATCHING);

    fc_object.device_driver->adc_read = &vt_adc_read_slow_exponential_fall_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_NOT_MATCHING);

    fc_object.device_driver->adc_read = &vt_adc_read_fast_exponential_fall_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_NOT_MATCHING);

    fc_object.device_driver->adc_read = &vt_adc_read_half_exponential_half_linear_fall_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_NOT_MATCHING);

    fc_object.device_driver->adc_read = &vt_adc_read_exponential_fall_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_MATCHING);

    fc_object.device_driver->adc_read = &vt_adc_read_different_amplitude_exponential_fall_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_MATCHING);

    fc_object.device_driver->tick_init = &vt_tick_init_with_some_values;
    fc_object.device_driver->adc_read  = &vt_adc_read_exponential_fall_voltage_response;
    vt_fallcurve_object_sensor_status(&fc_object, &sensor_status, &sensor_drift);
    assert_int_equal(sensor_status, VT_SIGNATURE_MATCHING);
}

VT_INT test_vt_fc_object_sensor()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_fallcurve_object_sensor_calibrate_recalibrate),
        cmocka_unit_test(test_vt_fallcurve_object_sensor_status),
    };

    return cmocka_run_group_tests_name("vt_fc_object_sensor", tests, NULL, NULL);
}
