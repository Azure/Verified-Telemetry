/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "test_vt_cs_definitions.h"

#include "vt_cs_api.h"
#include "vt_cs_config.h"

#include "cmocka.h"

#define TEST_REPEATING_RAW_SIGNATURE_0_SAMPLING_FREQ 5000.0f
#define TEST_REPEATING_RAW_SIGNATURE_1_SAMPLING_FREQ 12.8000f
#define TEST_REPEATING_RAW_SIGNATURE_2_SAMPLING_FREQ 15.2224f
#define TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH   128

#define TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ 1000
#define TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH 128
static int complete_read_counter = 0;
// clang-format off
static VT_UINT repeating_raw_signature_5000_hz[TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH] = {21,  22, 12, 14, 22, 15, 16, 18, 19, 23, 25, 15, 9, 5, 13, 16, 23, 22, 19, 19, 20, 21, 23, 23, 19, 23, 18, 18, 17, 16, 17, 23, 13, 13, 18, 14, 18, 19, 16, 6, 6, 22, 24, 26, 31, 22, 21, 18, 24, 23, 17, 13, 15, 22, 17, 16, 16, 21, 17, 19, 19, 23, 22, 26, 12, 8, 3, 5, 17, 28, 20, 20, 21, 21, 24, 28, 20, 19, 19, 18, 19, 15, 14, 13, 18, 21, 15, 16, 20, 17, 17, 14, 7, 6, 12, 22, 34, 24, 21, 19, 19, 23, 17, 15, 18, 16, 19, 16, 17, 19, 22, 18, 17, 20, 19, 22, 23, 11, 5, 7, 6, 13, 22, 28, 23, 23, 22, 24};
static VT_UINT repeating_raw_signature_12_hz[TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH] = {41, 47, 42, 53, 45, 60, 50, 56, 23, 25, 24, 29, 26, 18, 21, 36, 28, 23, 26, 29, 22, 22, 18, 30, 16, 16, 55, 54, 55, 55, 57, 59, 50, 47, 56, 25, 30, 21, 17, 22, 11, 32, 24, 22, 32, 28, 18, 25, 13, 24, 18, 26, 54, 63, 52, 59, 51, 60, 47, 46, 46, 23, 25, 24, 29, 26, 18, 21, 36, 28, 23, 26, 29, 22, 22, 18, 30, 16, 16, 55, 54, 55, 55, 57, 59, 50, 47, 56, 25, 30, 21, 17, 22, 11, 32, 24, 22, 32, 28, 18, 25, 13, 24, 18, 26, 54, 63, 52, 59, 51, 60, 47, 46, 46, 23, 25, 24, 29, 26, 18, 21, 36, 28, 23, 26, 29, 22, 22};
static VT_UINT repeating_raw_signature_15_hz[TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH] = {41, 47, 42, 53, 45, 60, 50, 56, 23, 25, 24, 29, 26, 18, 21, 36, 28, 23, 26, 29, 22, 22, 18, 30, 16, 16, 55, 54, 55, 55, 57, 59, 50, 47, 56, 25, 30, 21, 17, 22, 11, 32, 24, 22, 32, 28, 18, 25, 13, 24, 18, 26, 54, 63, 52, 59, 51, 60, 47, 46, 46, 23, 25, 24, 29, 26, 18, 21, 36, 28, 23, 26, 29, 22, 22, 18, 30, 16, 16, 55, 54, 55, 55, 57, 59, 50, 47, 56, 25, 30, 21, 17, 22, 11, 32, 24, 22, 32, 28, 18, 25, 13, 24, 18, 26, 54, 63, 52, 59, 51, 60, 47, 46, 46, 23, 25, 24, 29, 26, 18, 21, 36, 28, 23, 26, 29, 22, 22};
static VT_UINT non_repeating_raw_signature[TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH] = {40, 31, 31, 28, 36, 32, 31, 45, 35, 28, 35, 43, 32, 31, 38, 41, 30, 29, 33, 42, 31, 29, 22, 40, 30, 28, 35, 39, 32, 30, 29, 38, 32, 28, 33, 38, 29, 30, 35, 37, 32, 31, 41, 39, 29, 31, 26, 38, 29, 32, 42, 58, 62, 65, 75, 64, 61, 55, 69, 61, 65, 74, 63, 63, 76, 62, 59, 74, 65, 28, 27, 36, 30, 36, 38, 28, 35, 39, 27, 33, 42, 30, 34, 42, 32, 32, 44, 32, 30, 44, 31, 30, 42, 31, 30, 43, 32, 29, 26, 33, 28, 39, 33, 29, 34, 36, 29, 27, 36, 29, 35, 38, 30, 34, 42, 31, 33, 42, 31, 32, 44, 29, 30, 45, 33, 30, 41, 33};
static VT_FLOAT sampling_frequencies_template[VT_CS_MAX_SIGNATURES] = {5000.0f, 12.8000f, 15.2224f, 323.34f, 0.48f};
// clang-format on

VT_CURRENTSENSE_OBJECT cs_object;
VT_DEVICE_DRIVER device_driver;
VT_SENSOR_HANDLE sensor_handle;
VT_CURRENTSENSE_RAW_SIGNATURES_READER raw_signatures_reader;

static VT_VOID vt_adc_buffer_read_with_real_func(VT_ADC_ID adc_id,
    VT_ADC_CONTROLLER* adc_controller,
    VT_ADC_CHANNEL* adc_channel,
    VT_FLOAT* adc_read_buffer,
    VT_UINT buffer_length,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* set_sampling_frequency,
    VT_ADC_BUFFER_READ_CALLBACK_FUNC vt_adc_buffer_read_conv_half_cplt_callback,
    VT_ADC_BUFFER_READ_FULL_CALLBACK_FUNC vt_adc_buffer_read_conv_cplt_callback)
{
    for (int iter = 0; iter <= ((buffer_length / 2) + 6); iter++)
    {
        adc_read_buffer[iter] = 1;
    }
    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = ((buffer_length / 2) + 6);
    vt_adc_buffer_read_conv_half_cplt_callback();

    for (int iter = buffer_length / 2; iter <= buffer_length; iter++)
    {
        adc_read_buffer[iter] = 1;
    }
    if (complete_read_counter == 0)
    {
        complete_read_counter++;
        vt_adc_buffer_read_conv_cplt_callback(1);
    }
    return;
}

static VT_VOID vt_adc_buffer_read(VT_ADC_ID adc_id,
    VT_ADC_CONTROLLER* adc_controller,
    VT_ADC_CHANNEL* adc_channel,
    VT_FLOAT* adc_read_buffer,
    VT_UINT buffer_length,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* set_sampling_frequency,
    VT_ADC_BUFFER_READ_CALLBACK_FUNC vt_adc_buffer_read_conv_half_cplt_callback,
    VT_ADC_BUFFER_READ_FULL_CALLBACK_FUNC vt_adc_buffer_read_conv_cplt_callback)
{
    return;
}

static VT_UINT calculate_fft_ranges()
{
    VT_UINT ranges = 1;
    VT_FLOAT f_low = (VT_CS_ADC_MAX_SAMPLING_FREQ / 2) / VT_CS_SAMPLE_LENGTH;
    while (true)
    {
        if (f_low < VT_CS_ADC_MIN_SAMPLING_FREQ)
        {
            break;
        }
        f_low /= (((VT_FLOAT)VT_CS_SAMPLE_LENGTH) / 2.0f);
        ranges++;
    }
    return ranges;
}

// vt_currentsense_object_signature_read()
static VT_VOID test_vt_currentsense_object_signature_read(VT_VOID** state)
{

    float ref_voltage                   = 5;
    uint16_t adc_res                    = 12;
    float mv_to_ma                      = 1;
    sensor_handle.adc_ref_volt          = &ref_voltage;
    sensor_handle.adc_resolution        = &adc_res;
    sensor_handle.currentsense_mV_to_mA = mv_to_ma;
    sensor_handle.gpio_id               = 1;
    sensor_handle.adc_id                = 1;

    raw_signatures_reader.non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    raw_signatures_reader.non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;

    cs_object.device_driver         = &device_driver;
    cs_object.sensor_handle         = &sensor_handle;
    cs_object.raw_signatures_reader = &raw_signatures_reader;
    // VT_CHAR raw_signatures_buffer[sizeof(VT_CURRENTSENSE_RAW_SIGNATURES_READER)] = {0};

    cs_object.raw_signatures_reader_initialized = true;
    // cs_object.raw_signatures_reader             = (VT_CURRENTSENSE_RAW_SIGNATURES_READER*)raw_signatures_buffer;
    cs_object.device_driver->adc_buffer_read = &vt_adc_buffer_read;

    cs_object.fingerprintdb.template_type                                    = VT_CS_REPEATING_SIGNATURE;
    cs_object.fingerprintdb.template.repeating_signatures.num_signatures     = 0;
    cs_object.fingerprintdb.template.repeating_signatures.lowest_sample_freq = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current     = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_on     = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_off    = VT_DATA_NOT_AVAILABLE;
    cs_object.mode                                                           = VT_MODE_RUNTIME_EVALUATE;

    vt_currentsense_object_signature_read(&cs_object, 0);

    assert_int_equal(cs_object.raw_signatures_reader->num_repeating_raw_signatures, 0);

    cs_object.fingerprintdb.template.repeating_signatures.num_signatures = VT_CS_MAX_SIGNATURES;
    for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
    {
        cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq =
            sampling_frequencies_template[iter];
    }
    /*
            vt_currentsense_object_signature_read(&cs_object, 0);

            assert_int_equal(cs_object.raw_signatures_reader->num_repeating_raw_signatures, 0x4);
            for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
            {
                assert_float_equal(cs_object.raw_signatures_reader->repeating_raw_signatures[iter].sampling_frequency,
                    cs_object.fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq,
                    0.1);
            }
    */
    cs_object.fingerprintdb.template_type = VT_CS_NON_REPEATING_SIGNATURE;
    vt_currentsense_object_signature_read(&cs_object, 0);
    assert_int_equal(cs_object.raw_signatures_reader->num_repeating_raw_signatures, 0x4);
    assert_int_equal(
        cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency, VT_CS_ADC_MAX_SAMPLING_FREQ);

    cs_object.mode = VT_MODE_CALIBRATE;
    vt_currentsense_object_signature_read(&cs_object, 0);
    assert_int_equal(cs_object.raw_signatures_reader->num_repeating_raw_signatures, calculate_fft_ranges());

    cs_object.mode                           = VT_MODE_RUNTIME_EVALUATE;
    cs_object.device_driver->adc_buffer_read = &vt_adc_buffer_read_with_real_func;
    vt_currentsense_object_signature_read(&cs_object, 0);
}

// vt_currentsense_object_signature_process()
static VT_VOID test_vt_currentsense_object_signature_process(VT_VOID** state)
{
    VT_CURRENTSENSE_OBJECT cs_object;
    VT_CHAR raw_signatures_buffer[sizeof(VT_CURRENTSENSE_RAW_SIGNATURES_READER)] = {0};

    cs_object.raw_signatures_reader_initialized = true;
    cs_object.raw_signatures_reader             = (VT_CURRENTSENSE_RAW_SIGNATURES_READER*)raw_signatures_buffer;

    cs_object.fingerprintdb.template_type                                    = VT_CS_REPEATING_SIGNATURE;
    cs_object.fingerprintdb.template.repeating_signatures.num_signatures     = 0;
    cs_object.fingerprintdb.template.repeating_signatures.lowest_sample_freq = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current     = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_on     = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_off    = VT_DATA_NOT_AVAILABLE;

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;
    cs_object.fingerprintdb.template_type                                       = VT_CS_NON_REPEATING_SIGNATURE;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);
    // leed to look into cases of mode calib
    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_CALIBRATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.db_updated                                                        = false;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_0_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_1_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_12_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }
    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.mode, VT_MODE_CALIBRATE);
    assert_int_equal(cs_object.db_updated, true);
    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_NO_SIGNATURE_VALID);
    assert_int_equal(cs_object.fingerprintdb.template.repeating_signatures.num_signatures, 0);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RECALIBRATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.db_updated                                                        = false;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_0_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_1_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_12_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }
    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.mode, VT_MODE_RUNTIME_EVALUATE);
    assert_int_equal(cs_object.db_updated, true);
    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_NO_SIGNATURE_VALID);
    assert_int_equal(cs_object.fingerprintdb.template.repeating_signatures.num_signatures, 2);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    assert_int_equal(cs_object.mode, VT_MODE_RUNTIME_EVALUATE);
    assert_int_equal(cs_object.db_updated, true);
    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_NO_SIGNATURE_VALID);
    assert_int_equal(cs_object.fingerprintdb.template.repeating_signatures.num_signatures, 2);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] = 0;
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] = 0;
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_CALIBRATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.db_updated                                                        = false;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_0_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] = repeating_raw_signature_5000_hz[0];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_1_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] = repeating_raw_signature_12_hz[0];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }
    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.mode, VT_MODE_CALIBRATE);
    // assert_int_equal(cs_object.db_updated, true);
    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_NO_SIGNATURE_VALID);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RECALIBRATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.db_updated                                                        = false;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_0_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] = repeating_raw_signature_5000_hz[0];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_1_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] = repeating_raw_signature_12_hz[0];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }
    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.mode, VT_MODE_RUNTIME_EVALUATE);
    assert_int_equal(cs_object.db_updated, true);
    assert_int_equal(cs_object.fingerprintdb.template_type, VT_CS_NO_SIGNATURE_VALID);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[0];
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = 0;
    }

    vt_currentsense_object_signature_process(&cs_object);

    assert_int_equal(cs_object.sensor_status, VT_SIGNATURE_DB_EMPTY);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = false;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;
    cs_object.fingerprintdb.template_type                                       = VT_CS_NON_REPEATING_SIGNATURE;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    cs_object.fingerprintdb.template_type                                    = VT_CS_REPEATING_SIGNATURE;
    cs_object.fingerprintdb.template.repeating_signatures.num_signatures     = 0;
    cs_object.fingerprintdb.template.repeating_signatures.lowest_sample_freq = 250;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current     = 1;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_on     = 40;
    cs_object.fingerprintdb.template.non_repeating_signature.avg_curr_off    = 60;

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;

    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[0].sampling_freq;
    ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] =
            repeating_raw_signature_5000_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        cs_object.fingerprintdb.template.repeating_signatures.signatures[1].sampling_freq;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] =
            repeating_raw_signature_15_hz[iter1];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_RUNTIME_EVALUATE;
    cs_object.fingerprintdb.template_type == VT_CS_REPEATING_SIGNATURE;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current = VT_DATA_NOT_AVAILABLE;
    cs_object.fingerprintdb.template.repeating_signatures.num_signatures = 1;
    vt_currentsense_object_signature_process(&cs_object);

    cs_object.fingerprintdb.template.repeating_signatures.num_signatures = 0;
    cs_object.fingerprintdb.template.repeating_signatures.offset_current = 1;
    vt_currentsense_object_signature_process(&cs_object);

    cs_object.fingerprintdb.template_type == VT_CS_NON_REPEATING_SIGNATURE;
    cs_object.raw_signatures_reader->repeating_raw_signature_ongoing_collection = false;
    cs_object.mode                                                              = VT_MODE_CALIBRATE;
    cs_object.raw_signatures_reader_initialized                                 = true;
    cs_object.raw_signatures_reader->repeating_raw_signature_buffers_filled     = true;
    cs_object.db_updated                                                        = false;
    cs_object.raw_signatures_reader->num_repeating_raw_signatures               = 0;
    cs_object.raw_signatures_reader_initialized                                 = false;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_0_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[0].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[0].current_measured[iter1] = repeating_raw_signature_5000_hz[0];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sampling_frequency =
        TEST_REPEATING_RAW_SIGNATURE_1_SAMPLING_FREQ;
    cs_object.raw_signatures_reader->repeating_raw_signatures[1].sample_length = TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    for (VT_UINT iter1 = 0; iter1 < TEST_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->repeating_raw_signatures[1].current_measured[iter1] = repeating_raw_signature_12_hz[0];
    }
    cs_object.raw_signatures_reader->num_repeating_raw_signatures++;

    cs_object.raw_signatures_reader->non_repeating_raw_signature.num_datapoints = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sample_length  = TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH;
    cs_object.raw_signatures_reader->non_repeating_raw_signature.sampling_frequency =
        TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLING_FREQ;
    for (VT_UINT iter1 = 0; iter1 < TEST_NON_REPEATING_RAW_SIGNATURE_SAMPLE_LENGTH; iter1++)
    {
        cs_object.raw_signatures_reader->non_repeating_raw_signature.current_measured[iter1] = non_repeating_raw_signature[iter1];
    }

    vt_currentsense_object_signature_process(&cs_object);

    cs_object.mode = VT_MODE_RUNTIME_EVALUATE;
    vt_currentsense_object_signature_process(&cs_object);
}

VT_INT test_vt_cs_object_signature()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vt_currentsense_object_signature_process),
        cmocka_unit_test(test_vt_currentsense_object_signature_read),
    };

    return cmocka_run_group_tests_name("vt_cs_object_signature", tests, NULL, NULL);
}
