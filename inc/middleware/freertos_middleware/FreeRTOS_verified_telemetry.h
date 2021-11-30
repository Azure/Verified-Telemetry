/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef FREERTOS_VERIFIED_TELEMETRY_H
#define FREERTOS_VERIFIED_TELEMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Azure Provisioning/IoT Hub library includes */
#include "FreeRTOS_vt_fallcurve_component.h"
#include "FreeRTOS_vt_currentsense_component.h"
#include "vt_defs.h"

#define FREERTOS_AZURE_IOT_PNP_PROPERTIES 0x00000004

#define VT_SIGNATURE_TYPE_FALLCURVE    0x01
#define VT_SIGNATURE_TYPE_CURRENTSENSE 0x02

#define VT_MINIMUM_BUFFER_SIZE_BYTES sizeof(VT_CURRENTSENSE_RAW_SIGNATURES_READER)

union FreeRTOS_VT_SIGNATURE_COMPONENT_UNION_TAG {
    /* FallCurve Component */
    FreeRTOS_VT_FALLCURVE_COMPONENT fc;
    FreeRTOS_VT_CURRENTSENSE_COMPONENT cs;
};

typedef struct FreeRTOS_VT_OBJECT_TAG
{
    /* Signature Object Union */
    union FreeRTOS_VT_SIGNATURE_COMPONENT_UNION_TAG component;

    /* Signature Type */
    UINT signature_type;

    /* Pointer to next component */
    void* next_component;

} FreeRTOS_VT_OBJECT;

typedef struct FreeRTOS_VERIFIED_TELEMETRY_DB_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[VT_COMPONENT_NAME_MAX_LENGTH];

    UINT component_name_length;

    /* Pointer to first component */
    void* first_component;

    /* Pointer to last component */
    void* last_component;

    /* Number of component*/
    UINT components_num;

    /* Device specific implementations*/
    VT_DEVICE_DRIVER* device_driver;

    /* Enable Verified Telemetry*/
    bool enable_verified_telemetry;

    /* Device Status*/
    bool device_status;

    /* Device Status Property Sent*/
    bool device_status_property_sent;

    /* Pointer to byte buffer passed from application layer, used for fingerprint calculation/storage */
    CHAR* scratch_buffer;

    /* Length of byte buffer passed from application layer, used for fingerprint calculation/storage */
    UINT scratch_buffer_length;

} FreeRTOS_VERIFIED_TELEMETRY_DB;

/**
 * @brief Initializes Global Verified Telemetry using platform specific device drivers
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] component_name_ptr Name of the PnP component. Example - "vTDevice"
 * @param[in] enable_verified_telemetry User specified value to set Verified Telemetry active or inactive, can also be configured
 * during runtime from a writable Digital Twin property.
 * @param[in] device_driver The platform specific device driver components for interacting with the device hardware.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_init(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    UCHAR* component_name_ptr,
    bool enable_verified_telemetry,
    VT_DEVICE_DRIVER* device_driver,
    CHAR* scratch_buffer,
    UINT scratch_buffer_length);

/**
 * @brief Initializes Verified Telemetry for a particular sensor data stream
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] handle Pointer to variable of type NX_VT_OBJECT storing collection settings and configuration data for a particular
 * sensor telemetry.
 * @param[in] component_name_ptr Name of the sensor.  Example - "accelerometer" This would be prepended with 'vT' by VT library
 * @param[in] signature_type One of the defined signature types. Currently available types - VT_SIGNATURE_TYPE_FALLCURVE
 * @param[in] associated_telemetry Telmetries associated with this sensor, separated by commas  Example - "accelerometerX,
 * accelerometerY, accelerometerZ"
 * @param[in] telemetry_status_auto_update User specified value to control whether fingerprint computation for the sensor should
 * be invoked when nx_vt_compute_evaluate_fingerprint_all_sensors is called
 * @param[in] sensor_handle The sensor specific connection configuration for collecting VT signatures.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_signature_init(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    FreeRTOS_VT_OBJECT* handle,
    UCHAR* component_name_ptr,
    UINT signature_type,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update,
    VT_SENSOR_HANDLE* sensor_handle);

/**
 * @brief Creates and sends telemetry JSON with message properties containing telemetry status
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] xAzureIoTHubClient Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] telemetry_data Pointer to buffer containing telemetry data.
 * @param[in] data_size Length of telemetry data passed in the buffer.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_verified_telemetry_message_create_send(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    const UCHAR* telemetry_data,
    UINT data_size);

/**
 * @brief Processes all commands supported by VT Middleware.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] xAzureIoTHubClient Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] pnp_command_name_ptr Name of the command invoked.
 * @param[in] pnp_command_name_length Length of name of the command invoked.
 * @param[out] status_code Status updated by function based on command execution.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_process_command(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    UINT* status_code);

/**
 * @brief Updates Digital Twin with default desired property values when device is booted for the first time
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] xAzureIoTHubClient Pointer to initialized Azure IoT PnP instance.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_properties(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, AzureIoTHubClient_t* xAzureIoTHubClient);

/**
 * @brief Processes all desired property updates supported by vT Middleware.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] xAzureIoTHubClient Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] xReader Pointer to read the JSON payload of command.
 * @param[in] version Property version stored in digital twin.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_process_property_update(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    AzureIoTHubClient_t* xAzureIoTHubClient,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* xReader,
    UINT version);

/**
 * @brief Computes status of all telemetries which have been initialized to provide Verified Telemetry
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_compute_evaluate_fingerprint_all_sensors(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB);

/**
 * @brief Updates Digital Twin with default desired property values when device is booted for the first time
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] xAzureIoTHubClient Pointer to initialized Azure IoT PnP instance.
 * @param[in] message_type Type of document, only valid value are NX_AZURE_IOT_PNP_DESIRED_PROPERTIES or
 * NX_AZURE_IOT_PNP_PROPERTIES
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_send_desired_property_after_boot(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, AzureIoTHubClient_t* xAzureIoTHubClient, UINT message_type);

/**
 * @brief Synchronizes VT Settings stored in digital Twin as reported properties at startup.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] json_reader_ptr Pointer to read the JSON payload of command.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

AzureIoTResult_t FreeRTOS_vt_process_reported_property_sync(FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    AzureIoTJSONReader_t* json_reader_ptr);

AzureIoTResult_t FreeRTOS_vt_signature_read(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, UCHAR* associated_telemetry, UINT associated_telemetry_length);

UINT FreeRTOS_vt_signature_process(
    FreeRTOS_VERIFIED_TELEMETRY_DB* verified_telemetry_DB, UCHAR* associated_telemetry, UINT associated_telemetry_length);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_VERIFIED_TELEMETRY_H */
