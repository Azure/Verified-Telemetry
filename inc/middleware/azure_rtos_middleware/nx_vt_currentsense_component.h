/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/** @file */

#ifndef NX_VT_CURRENTSENSE_COMPONENT_H
#define NX_VT_CURRENTSENSE_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_pnp_client.h"

#include "vt_cs_api.h"

typedef struct NX_VT_CURRENTSENSE_COMPONENT_TAG
{
    /* Name of this component */
    UCHAR component_name_ptr[30];

    /* Component Name Length */
    UINT component_name_length;

    /* Telemetry associated with the currentsense signature component*/
    UCHAR associated_telemetry[100];

    /* Currentsense Object */
    VT_CURRENTSENSE_OBJECT cs_object;

    /* Status of reported properties sent  */
    UINT property_sent;

} NX_VT_CURRENTSENSE_COMPONENT;

/**
 * @brief Initializes an instance of Verified Telemetry Information Interface for currentsense component.
 *
 * @param[in] handle The handle created by a call to the initialization function.
 * @param[in] component_name_ptr Name of the PNP component eg. vTsoilMoistureExternal1.
 * @param[in] device_driver The platform driver functions for interacting with the device hardware.
 * @param[in] sensor_handle The platform driver references for interacting with the particular sensor connected to device
 * hardware.
 * @param[in] associated_telemetry Name of the telemetry associated with this component.
 * @param[in] shared_buffer Pointer to byte buffer passed from application layer, used for fingerprint calculation/storage.
 * @param[in] shared_buffer_length Length of byte buffer passed from application layer, used for fingerprint calculation/storage.
 *
 * @retval VT_SUCCESS upon success or an error code upon failure.
 */

UINT nx_vt_currentsense_init(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* component_name_ptr,
    VT_DEVICE_DRIVER* device_driver,
    VT_SENSOR_HANDLE* sensor_handle,
    UCHAR* associated_telemetry,
    CHAR* shared_buffer,
    UINT shared_buffer_size);

/**
 * @brief Send read-only properties of the currentsense component like telemetryStatus, fingerprintType, fingerprintTemplate,
 * fingerprintTemplateConfidenceMetric to IoT Hub.
 *
 * @param[in] handle The currentsense handle created by a call to the initialization function.
 * @param[in] iotpnp_client_ptr Pointer to initialized Azure IoT PnP instance.
 * @param[out] device_status Pointer to update the global bool showing if all devices are working or not.
 * @param[in] toggle_verified_telemetry Bool value to enable VT for this component or not.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

UINT nx_vt_currentsense_reported_properties(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    bool* device_status,
    bool toggle_verified_telemetry);

/**
 * @brief Process commands related to currentsense component eg. setResetFingerprintTemplate , retrainFingerprintTemplate.
 *
 * @param[in] handle The currentsense handle created by a call to the initialization function.
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

UINT nx_vt_currentsense_process_command(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    UCHAR* component_name_ptr,
    UINT component_name_length,
    UCHAR* pnp_command_name_ptr,
    UINT pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER* json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER* json_response_ptr,
    UINT* status_code);

/**
 * @brief Synchronizes on-device currentsense template database from the digital twin upon startup.
 *
 * @param[in] handle The currentsense handle created by a call to the initialization function.
 * @param[in] iotpnp_client_ptr Pointer to initialized Azure IoT PnP instance.
 * @param[in] component_name_ptr Name of the component.
 * @param[in] component_name_length Length of name of the component.
 * @param[in] name_value_reader_ptr Pointer to read the JSON payload of command.
 * @param[in] version Property version stored in digital twin.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

UINT nx_vt_currentsense_process_reported_property_sync(NX_VT_CURRENTSENSE_COMPONENT* handle,
    NX_AZURE_IOT_PNP_CLIENT* iotpnp_client_ptr,
    const UCHAR* component_name_ptr,
    UINT component_name_length,
    NX_AZURE_IOT_JSON_READER* name_value_reader_ptr,
    UINT version);

/**
 * @brief Start reading current on the device. Call this function just before reading sensor data.
 *
 * @param[in] handle The currentsense handle created by a call to the initialization function.
 * @param[in] associated_telemetry Name of the telemetry associated with this component.
 * @param[in] associated_telemetry_length Length of the name of the telemetry associated with this component.
 * @param[in] toggle_verified_telemetry Bool value to enable VT for this component or not.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

UINT nx_vt_currentsense_signature_read(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry,UINT mode);

/**
 * @brief Process the raw current data stored. Get status in normal operation, calibrate and recalibrate based on commands.
 *
 * @param[in] handle The currentsense handle created by a call to the initialization function.
 * @param[in] associated_telemetry Name of the telemetry associated with this component.
 * @param[in] associated_telemetry_length Length of the name of the telemetry associated with this component.
 * @param[in] toggle_verified_telemetry Bool value to enable VT for this component or not.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

UINT nx_vt_currentsense_signature_process(NX_VT_CURRENTSENSE_COMPONENT* handle,
    UCHAR* associated_telemetry,
    UINT associated_telemetry_length,
    bool toggle_verified_telemetry);

/**
 * @brief Get status of the sensor related to this currentsense component.
 *
 * @param[in] handle The currentsense handle created by a call to the initialization function.
 * @param[in] toggle_verified_telemetry Bool value to enable VT for this component or not.
 *
 * @retval NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.
 */

bool nx_vt_currentsense_fetch_telemetry_status(NX_VT_CURRENTSENSE_COMPONENT* handle, bool toggle_verified_telemetry);

#ifdef __cplusplus
}
#endif
#endif /* NX_VT_CURRENTSENSE_COMPONENT_H */
