# Verified Telemetry API

## Overview

The following API documentation outlines the structure for both the Azure RTOS and the FreeRTOS Verified Telemetry SDK:

> * [VT SDK](https://azure.github.io/Verified-Telemetry)
> * [VT Azure RTOS SDK](https://azure.github.io/Verified-Telemetry/nx__verified__telemetry_8h.html)
> * [VT FreeRTOS SDK](https://azure.github.io/Verified-Telemetry)

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

    ```shell
    git submodule add https://github.com/Azure/Verified-Telemetry.git core/lib/verified-telemetry
    ```

1. Include library files in [getting-started/MXChip/AZ3166/lib/CMakeLists.txt](https://github.com/azure-rtos/getting-started/blob/master/MXChip/AZ3166/lib/CMakeLists.txt)

    ```cmake
    add_subdirectory(${CORE_LIB_DIR}/verified-telemetry verified_telemetry)
    ```

### Step 2 - Initialize Verified Telemetry

1. Global Verified Telemetry needs to be initialized with the device specific platform functions
1. Similarly, to initalize Verified Telemetry for a particular sensor/telemetry the details of sensor connection must be passed using the nx_vt_signature_init function
1. An example of usage of [nx_vt_init](https://azure.github.io/Verified-Telemetry/nx__verified__telemetry_8h.html) and [nx_vt_signature_init](https://azure.github.io/Verified-Telemetry/nx__verified__telemetry_8h.html#af097ec10f7efe9e8ef6505166938b46c) functions is shown below
1. Include headers for VT
    ```c
    #include "nx_verified_telemetry.h"
    ```

1. Include header which define platform specific device drivers
    ```c
    #include "sample_vt_device_driver.h"
    ```

1. Define Verified Telemetry DB which will store configuration, platform specific device drivers and runtime information
    ```c
    static NX_VERIFIED_TELEMETRY_DB verified_telemetry_DB;
    ```

1. Define variable of VT_DEVICE_DRIVER type which will contain pointers to platform specific functions required by VT
    ```c
    static VT_DEVICE_DRIVER sample_device_driver;
    ```

1. Define scratch buffer of length VT_RECOMMENDED_BUFFFER_SIZE_BYTES (2500 bytes). Thus buffer would be used by the library to store runtime signatures
    ```c
    static char scratch_buffer[VT_RECOMMENDED_BUFFFER_SIZE_BYTES];
    ```

1. Define a variables of NX_VT_OBJECT type for each telemetry that will be supporting Verified Telemetry
    ```c
    static NX_VT_OBJECT sample_signature_sensor_1;
    // static NX_VT_OBJECT sample_signature_sensor_2;
    static NX_VT_OBJECT sample_signature_sensor_N;
    ```

1. Define variables of VT_SENSOR_HANDLE type which will contain connection information for each sensor
    ```c
    static VT_SENSOR_HANDLE sample_handle_sensor_1;
    // static VT_SENSOR_HANDLE sample_handle_sensor_2;
    static VT_SENSOR_HANDLE sample_handle_sensor_N;
    ```

1. Define a Verified Telemetry User Initialization wrapper function which will make calls to necessary initialization function from the vT library and return pointer to the configured NX_VERIFIED_TELEMETRY_DB variable
    ```c
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
    ```

1. Call the Verified Telemetry User Initialization wrapper function and update verified_telemetry_DB
    ```c
    verified_telemetry_DB = sample_pnp_verified_telemetry_user_init();
    ```

1. A complete sample can be found [here](https://github.com/Azure/Verified-Telemetry-Device-Sample/blob/main/MXChip/AZ3166/app/sample_nx_verified_telemetry_init.c)

### Step 3 - Use VT Middleware APIs based on the following guidelines

1. Use this function to initialize VT PnP Components
    ```c
    nx_vt_azure_iot_pnp_client_component_add(verified_telemetry_DB, iotpnp_client_ptr)
    ```

1. Call these functions once after boot
    ```C
    nx_vt_send_desired_property_after_boot(verified_telemetry_DB, pnp_client_ptr, message_type)
    ```

    ```c
    nx_vt_process_reported_property_sync(
        verified_telemetry_DB, pnp_client_ptr, component_ptr, component_len, &name_value_reader, version)
    ```

1. Call these functions at fixed time intervals
    ```c
    nx_vt_properties(verified_telemetry_DB, &(context->iotpnp_client))
    ```

    ```c
    nx_vt_compute_evaluate_fingerprint_all_sensors(verified_telemetry_DB)
    ```

1. Use this function to send telemetry
    ```c
    nx_vt_verified_telemetry_message_create_send(handle->verified_telemetry_DB,
        iotpnp_client_ptr,
        handle->component_name_ptr,
        handle->component_name_length,
        NX_WAIT_FOREVER,
        (UCHAR*)scratch_buffer,
        buffer_length)
    ```

1. Desired Property Callback
    ```c
    nx_vt_process_property_update(
        verified_telemetry_DB, pnp_client_ptr, component_ptr, component_len, &name_value_reader, version)
    ```

1. Command Callback
    ```c
    nx_vt_process_command(verified_telemetry_DB,
        &(sample_context_ptr->iotpnp_client),
        UCHAR*)component_name_ptr,
        component_name_length,
        (UCHAR*)pnp_command_name_ptr,
        pnp_command_name_length,
        &json_reader,
        &json_writer,
        &status_code))
    ```
