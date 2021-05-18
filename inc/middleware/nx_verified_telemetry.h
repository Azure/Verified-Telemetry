/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef NX_VERIFIED_TELEMETRY_H
#define NX_VERIFIED_TELEMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_pnp_client.h"

/* Verified Telemetry Components */
#include "nx_vt_fallcurve_component.h"

#define VT_SIGNATURE_TYPE_FALLCURVE 0x01

union NX_VT_SIGNATURE_COMPONENT_UNION_TAG {
    /* FallCurve Component */
    NX_VT_FALLCURVE_COMPONENT fc;
};

typedef struct NX_VT_OBJECT_TAG
{
    /* Signature Object Union */
    union NX_VT_SIGNATURE_COMPONENT_UNION_TAG component;

    /* Signature Type */
    UINT signature_type;

    /* Pointer to next component */
    void* next_component;

} NX_VT_OBJECT;

typedef struct NX_VERIFIED_TELEMETRY_DB_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[30];

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

} NX_VERIFIED_TELEMETRY_DB;

/**
 * @brief Initializes Global Verified Telemetry.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] component_name_ptr Name of the PNP component.
 * @param[in] enableVerifiedTelemetry User specified value to set Verified Telemetry active or inactive, can also be configured
 * during runtime from a writable Digital Twin property.
 * @param[in] device_driver The platform driver components for interacting with the device hardware.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */
UINT nx_vt_init(
    void* verified_telemetry_DB, UCHAR* component_name_ptr, bool enable_verified_telemetry, VT_DEVICE_DRIVER* device_driver);

UINT nx_vt_signature_init(void* verified_telemetry_DB,
    NX_VT_OBJECT* handle,
    UCHAR* component_name_ptr,
    UINT signature_type,
    UCHAR* associated_telemetry,
    bool telemetry_status_auto_update,
    VT_SENSOR_HANDLE* sensor_handle);

/**
 * @brief Processes all commands supported by VT Middleware.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] iotpnp_client_ptr Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] pnp_command_name_ptr Name of the command invoked.
 * @param[in] pnp_command_name_length Length of name of the command invoked.
 * @param[in] json_reader_ptr Pointer to read the JSON payload of command.
 * @param[in] json_response_ptr Pointer to write response JSON payload.
 * @param[out] status_code Status updated by function based on command execution.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */
UINT nx_vt_process_command(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code);

/**
 * @brief Processes all desired property updates supported by vT Middleware.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] iotpnp_client_ptr Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] name_value_reader_ptr Pointer to read the JSON payload of command.
 * @param[in] version Property version stored in digital twin.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */
UINT nx_vt_process_property_update(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);

/**
 * @brief Creates payloads and sends all reported properties supported by VT Middleware.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] iotpnp_client_ptr Pointer to initialized Azure IoT PnP instance.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */
UINT nx_vt_properties(void* verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr);

UINT nx_vt_send_desired_property_after_boot(
    void* verified_telemetry_DB, NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr, UINT message_type);

/**
 * @brief Synchronizes VT Settings stored in digital Twin as reported properties at startup.
 *
 * @param[in] verified_telemetry_DB Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data.
 * @param[in] iotpnp_client_ptr Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] name_value_reader_ptr Pointer to read the JSON payload of command.
 * @param[in] version Property version stored in digital twin.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */
UINT nx_vt_process_reported_property_sync(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);

// VT Create and Send Telemetry JSON with message properties
UINT nx_vt_verified_telemetry_message_create_send(void* verified_telemetry_DB,
    NX_AZURE_IOT_PNP_CLIENT* pnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    UINT wait_option,
    const UCHAR* telemetry_data,
    UINT data_size);

// VT Compute and Evaluate Sensor Fingerprints
UINT nx_vt_compute_evaluate_fingerprint_all_sensors(void* verified_telemetry_DB);

// UINT nx_vt_compute_evaluate_fingerprint_one_sensor(void* verified_telemetry_DB, const UCHAR* component_name_ptr,
//     UINT component_name_length);

#ifdef __cplusplus
}
#endif

#endif /* NX_VERIFIED_TELEMETRY_H */
