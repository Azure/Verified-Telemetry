# Verified Telemetry API

## Overview

This is the companion API documentation for the Verified Telemetry [GitHub Repository](https://github.com/Azure/Verified-Telemetry).

The following API documentation outlines the structure for both the Azure RTOS and the FreeRTOS Verified Telemetry SDK:

* [Verified Telemetry Azure RTOS SDK](nx__verified__telemetry_8h.html)
* [Verified Telemetry FreeRTOS SDK](_free_r_t_o_s__verified__telemetry_8h.html)

## Library Structure

The Verified Telemetry library has been structured around the following components:

**[core](./src/core)**    
Core functions to support verified telemetry, such as functions to collect, validate and evaluate sensor fingerprints.

**[middleware](./src/middleware)**    
The implementations to support interactions with Azure IoT Middleware for Azure RTOS and FreeRTOS.

**[platform](./src/platform)**    
The hardware compatibility layer.

## File Structure

The file structure is as follows:

**inc/**    
Contains header files for core, middleware and platform components.

**src/**    
Contains the library implementations.

**PnPModel/**    
Contains sample Plug and Play device models to support Verified Telemetry.

## Azure RTOS integration guide

Developers can refer to the [Azure RTOS Device Samples](https://github.com/Azure/Verified-Telemetry-Device-Sample) for Verified Telemetry sample code.

### Step 1 - Include Verified Telemetry library

1. In the root folder, run the following command

        git submodule add https://github.com/Azure/Verified-Telemetry.git core/lib/verified-telemetry

1. Include library files in [getting-started/MXChip/AZ3166/lib/CMakeLists.txt](https://github.com/azure-rtos/getting-started/blob/master/MXChip/AZ3166/lib/CMakeLists.txt)

        add_subdirectory(${CORE_LIB_DIR}/verified-telemetry verified_telemetry)

### Step 2 - Initialize Verified Telemetry

1. Global Verified Telemetry needs to be initialized with the device specific platform functions
2. Similarly, to initialize Verified Telemetry for a particular sensor/telemetry the details of sensor connection must be passed using the nx_vt_signature_init function
3. An example of usage of [nx_vt_init](https://azure.github.io/Verified-Telemetry/nx__verified__telemetry_8h.html) and [nx_vt_signature_init](https://azure.github.io/Verified-Telemetry/nx__verified__telemetry_8h.html#af097ec10f7efe9e8ef6505166938b46c) functions is shown below
4. Include headers for VT

        #include "nx_verified_telemetry.h"

5. Include header which define platform specific device drivers

        #include "sample_vt_device_driver.h"

6. Define Verified Telemetry DB which will store configuration, platform specific device drivers and runtime information

        static NX_VERIFIED_TELEMETRY_DB verified_telemetry_DB;

7. Define variable of VT_DEVICE_DRIVER type which will contain pointers to platform specific functions required by VT

        static VT_DEVICE_DRIVER sample_device_driver;

8. Define scratch buffer of length VT_RECOMMENDED_BUFFER_SIZE_BYTES (2500 bytes). Thus buffer would be used by the library to store runtime signatures

        static char scratch_buffer[VT_RECOMMENDED_BUFFER_SIZE_BYTES];

9. Define a variables of NX_VT_OBJECT type for each telemetry that will be supporting Verified Telemetry

        static NX_VT_OBJECT sample_signature_sensor_1;
        // static NX_VT_OBJECT sample_signature_sensor_2;
        static NX_VT_OBJECT sample_signature_sensor_N;

10. Define variables of VT_SENSOR_HANDLE type which will contain connection information for each sensor

        static VT_SENSOR_HANDLE sample_handle_sensor_1;
        // static VT_SENSOR_HANDLE sample_handle_sensor_2;
        static VT_SENSOR_HANDLE sample_handle_sensor_N;

11. Define a Verified Telemetry User Initialization wrapper function which will make calls to necessary initialization function from the vT library and return pointer to the configured NX_VERIFIED_TELEMETRY_DB variable

        NX_VERIFIED_TELEMETRY_DB* sample_nx_verified_telemetry_user_init()
        {
            UINT status;

            sample_device_driver.adc_init          = &vt_adc_init;
            sample_device_driver.adc_read          = &vt_adc_read;
            sample_device_driver.gpio_on           = &vt_gpio_on;
            sample_device_driver.gpio_off          = &vt_gpio_off;
            sample_device_driver.tick_init         = &vt_tick_init;
            sample_device_driver.tick_deinit       = &vt_tick_deinit;
            sample_device_driver.tick              = &vt_tick;
            sample_device_driver.interrupt_enable  = &vt_interrupt_enable;
            sample_device_driver.interrupt_disable = &vt_interrupt_disable;

            if ((status = nx_vt_init(&verified_telemetry_DB, (UCHAR*)"vTDevice", true, &sample_device_driver, scratch_buffer)))
            {
                printf("Failed to configure Verified Telemetry settings: error code = 0x%08x\r\n", status);
            }

            sample_handle_sensor_1.adc_id         = vt_adc_id_sensor_1;
            sample_handle_sensor_1.adc_controller = (void*)&vt_adc_controller_sensor_1;
            sample_handle_sensor_1.adc_channel    = (void*)&vt_adc_channel_sensor_1;
            sample_handle_sensor_1.gpio_id        = vt_gpio_id_sensor_1;
            sample_handle_sensor_1.gpio_port      = (void*)vt_gpio_port_sensor_1;
            sample_handle_sensor_1.gpio_pin       = (void*)&vt_gpio_pin_sensor_1;

            if ((status = nx_vt_signature_init(&verified_telemetry_DB,
                &sample_signature_sensor_1,
                (UCHAR*)"soilMoistureExternal1",
                VT_SIGNATURE_TYPE_FALLCURVE,
                (UCHAR*)"soilMoistureExternal1",
                true,
                &sample_handle_sensor_1)))
            {
                printf("Failed to initialize VT for soilMoistureExternal1 telemetry: error code = 0x%08x\r\n", status);
            }

            return (&verified_telemetry_DB);
        }

12. Call the Verified Telemetry User Initialization wrapper function and update verified_telemetry_DB

        verified_telemetry_DB = sample_pnp_verified_telemetry_user_init();

13. A complete sample can be found [here](https://github.com/Azure/Verified-Telemetry-Device-Sample/blob/main/MXChip/AZ3166/app/sample_nx_verified_telemetry_init.c)

### Step 3 - Use VT Middleware APIs based on the following guidelines

1. Use this function to initialize VT PnP Components

        nx_vt_azure_iot_pnp_client_component_add(verified_telemetry_DB, iotpnp_client_ptr)

2. Call these functions once after boot

        nx_vt_send_desired_property_after_boot(verified_telemetry_DB, pnp_client_ptr, message_type)

        nx_vt_process_reported_property_sync(
            verified_telemetry_DB, pnp_client_ptr, component_ptr, component_len, &name_value_reader, version)

3. Call these functions at fixed time intervals

        nx_vt_properties(verified_telemetry_DB, &(context->iotpnp_client))

        nx_vt_compute_evaluate_fingerprint_all_sensors(verified_telemetry_DB)

4. Use this function to send telemetry

        nx_vt_verified_telemetry_message_create_send(handle->verified_telemetry_DB,
            iotpnp_client_ptr,
            handle->component_name_ptr,
            handle->component_name_length,
            NX_WAIT_FOREVER,
            (UCHAR*)scratch_buffer,
            buffer_length)

5. Desired Property Callback

        nx_vt_process_property_update(
            verified_telemetry_DB, pnp_client_ptr, component_ptr, component_len, &name_value_reader, version)

6. Command Callback

        nx_vt_process_command(verified_telemetry_DB,
            &(sample_context_ptr->iotpnp_client),
            UCHAR*)component_name_ptr,
            component_name_length,
            (UCHAR*)pnp_command_name_ptr,
            pnp_command_name_length,
            &json_reader,
            &json_writer,
            &status_code))
