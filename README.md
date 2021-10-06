
[![build](https://github.com/Azure/Verified-Telemetry/workflows/Build%20Core/badge.svg)](https://github.com/Azure/Verified-Telemetry/actions/workflows/build-core.yaml)
[![codecov](https://codecov.io/gh/Azure/verified-telemetry/branch/main/graph/badge.svg?token=7JFQ2EX3BV)](https://codecov.io/gh/Azure/verified-telemetry)

# Verified Telemetry for Azure IoT

## Overview

The quality of sensor data is fundamental for driving IoT adoption. Since IoT sensors are often made of low-cost components, deployed in-the-wild and in harsh environments, they are prone to failures leading to faulty or dirty data. The effect of this faulty data will lead to the well-known garbage-in / garbage-out process, leading to cascading ill-effects upstream and at times catastrophic decisions.  

Verified Telemetry (VT) is a state-of-the-art solution to determine the health of the sensor, i.e., working or faulty, which is consequently used to determine the quality of the sensed data. This is achieved by devising an intelligent “sensor fingerprint”, a set of unique electrical characteristics that differs between working and faulty sensors. The fingerprints can detect faults for a wide variety of off-the-shelf sensors and can be easily implemented with lightweight software code running on the IoT device. This novel approach empowers customers with a reliable and automated way to remotely measure and observe the health of the sensor in real-time alongside the data collected. The data associated with a validated fingerprint results in Verified Telemetry.

![VT_Overview](./docs/Azure_VT.PNG)

## API documentation

Refer to the [Verified Telemetry API documentation](https://azure.github.io/Verified-Telemetry/) for further details on the Azure RTOS and FreeRTOS Middleware APIs.

## Plug and Play Model

Verified Telemetry Library provides capabilities for interaction using a Plug and Play Model.
Details about this model can be found [here](./PnPModel).

## Resource Requirements

We expect Verified Telemetery code to run on microcontrollers, which have very limited amounts of flash and RAM. Below is our resource requirements to add verified telemetry code:

| Type      | Size  |
| --------- | ----- |
| **Flash** | 46 KB |
| **RAM**   | 14 KB |

## Samples

We provide multiple device and solution samples to showcase the usage of Verified Telemetry. Please follow the samples below to see Verified Telemetry in action.

To get you started with the device, refer to the following device samples for Azure RTOS and FreeRTOS:
* [Azure RTOS samples](https://github.com/Azure/Verified-Telemetry-Device-Sample)
* [FreeRTOS samples](https://github.com/Azure-Samples/Verified-Telemetry-FreeRTOS-Sample)

To get you started with the solution, refer to the following Grafana sample:
* [Grafana solution sample](https://github.com/Azure/Verified-Telemetry-Solution-Sample) showcases how Verified Telemetry can be utilized in real world scenarios.

## Library Structure

The Verified Telemetry library has been structured around the following components:

* [Core](./src/core): Core functions to support verified telemetry, such as functions to collect, validate and evaluate sensor fingerprints.
* [Middleware](./src/middleware): The implementations to support interactions with Azure IoT Middleware for Azure RTOS and FreeRTOS.
* [Platform](./src/platform): The hardware compatibility layer.

## File Structure

The file structure is as follows:

* **inc/** Contains header files for core, middleware and platform components.

* **src/** Contains implementations for all the header files in **/inc**.

* **PnPModel/** Contains implementations for sample PnP models to support Verified Telemetry.

* **tests/** Contains test cases for [VT Core](./src/core)

* **CMakeLists.txt**  
    CMake script to generate build files.

    Verified Telemetry provides several options to customize the library according to the project requirement:
    * `VT_MIDDLEWARE_RTOS` : Must be one of **AZURE_RTOS** or **FREERTOS**, depending on your RTOS choice.
    * `VT_UNIT_TESTING` : Setting this ON compiles [test cases](./tests) for [VT Core](./src/core) but disables [VT Middleware](./src/middleware)
    * `VT_CODE_COVERAGE` : Setting this ON calculates the code coverage of the [test cases](./tests) for [VT Core](./src/core)

## Platform level functions

Verified Telemetry requires platform specific functions to be defined at the application layer and then passed down to the library when VT is initialized.

* Refer to [vt_device_driver.h](./inc/platform/vt_device_driver.h) for function declarations
* Refer to [vt_device_driver_template.c](./templates/vt_device_driver_template.c) for a template for implementation of the platform functions.

The application developer must implement the following functions:

 | Function  | Implementation Requirements  | FallCurve Required? |
 | --------- | ---------------------------- | ------------------- |
 | adc_single_init   | Initialize ADC when the controller and channel are passed as arguments         | REQUIRED |
 | adc_single_read   | Return ADC data when the controller and channel are passed as arguments        | REQUIRED |
 | adc_buffer_read   | Return buffer filled with ADC data using DMA/non-blocking read when the controller, channel, sampling frequency and sample length are passed as arguments | REQUIRED |
 | gpio_on           | Set a GPIO pin HIGH when the GPIO port and pin are passed as arguments         | REQUIRED |
 | gpio_off          | Set a GPIO pin LOW when the GPIO port and pin are passed as arguments          | REQUIRED |
 | tick_init         | Initialize a tick counter with a time resolution that is passed as an argument | REQUIRED |
 | tick_deinit       | De-initialize a running tick counter                                           | REQUIRED |
 | tick              | Return the present tick value                                                  | REQUIRED |
 | interrupt_enable  | Enable global interrupts on the MCU                                            | OPTIONAL |
 | interrupt_disable | Disable global interrupts on the MCU                                           | OPTIONAL |

> **NOTE:** Samples for implementation of platform functions can also be found in the [Device samples](https://github.com/Azure/Verified-Telemetry-Device-Sample/blob/main/MXChip/AZ3166/app/sample_vt_device_driver.c).

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
