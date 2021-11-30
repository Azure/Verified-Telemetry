[![build](https://github.com/Azure/Verified-Telemetry/workflows/Build%20Core/badge.svg)](https://github.com/Azure/Verified-Telemetry/actions/workflows/build-core.yaml)
[![codecov](https://codecov.io/gh/Azure/verified-telemetry/branch/main/graph/badge.svg?token=7JFQ2EX3BV)](https://codecov.io/gh/Azure/verified-telemetry)

# Verified Telemetry for Azure IoT

## Overview

The quality of sensor data is fundamental for driving IoT adoption. Since IoT sensors are often made of low-cost components, deployed in-the-wild and in harsh environments, they are prone to failures leading to faulty or dirty data. The effect of this faulty data will lead to the well-known garbage-in / garbage-out process, leading to cascading ill-effects upstream and at times catastrophic decisions.  

Verified Telemetry (VT) is a state-of-the-art solution to determine the health of the sensor, i.e., working or faulty, which is consequently used to determine the quality of the sensed data. This is achieved by devising an intelligent “sensor fingerprint”, a set of unique electrical characteristics that differs between working and faulty sensors. The fingerprints can detect faults for a wide variety of off-the-shelf sensors and can be easily implemented with lightweight software code running on the IoT device. This novel approach empowers customers with a reliable and automated way to remotely measure and observe the health of the sensor in real-time alongside the data collected. The data associated with a validated fingerprint results in Verified Telemetry.

Verified Telemetry uses two technologies - FallCurve for analog sensors, CurrentSense for Digital Sensors. 

### Fall Curve

It reads the 

![VT_Overview](./docs/Azure_VT.PNG)

## API documentation

Refer to the [Verified Telemetry API](https://azure.github.io/Verified-Telemetry) for details on the VT Azure RTOS and FreeRTOS SDKs and how they are used.

## Resource Requirements

Verified Telemetry is designed to run on resource contrained MCUs, flash and RAM is limited. Below is our resource requirements for Verified Telemetry:

| Type      | Size  |
| --------- | ----- |
| **Flash** | 46 KB |
| **RAM**   | 14 KB |

## Plug and Play

Verified Telemetry Library provides capabilities for interaction using a Plug and Play Model.
Details about this model can be found [here](./PnPModel).

## Samples

We provide multiple device and solution samples to showcase the usage of Verified Telemetry. Please follow the samples below to see Verified Telemetry in action.

To get you started with the device, refer to the following device samples for Azure RTOS and FreeRTOS:
* [Azure RTOS samples](https://github.com/Azure/Verified-Telemetry-Device-Sample)
* [FreeRTOS samples](https://github.com/Azure-Samples/Verified-Telemetry-FreeRTOS-Sample)

To get you started with the solution, refer to the following Grafana sample:
* [Grafana solution sample](https://github.com/Azure/Verified-Telemetry-Solution-Sample) showcases how Verified Telemetry can be utilized in real world scenarios.

## Using Verified Telemetry

Verified Telemetry utilizes CMake for building the project. To integrate Verified Telemetry into your projects build system follow these steps:

1. Using Git, submodule VT into your project:

    ```shell
    git submodule add https://github.com/Azure/Verified-Telemetry.git
    ```
1. In your CMake, set `VT_MIDDLEWARE_RTOS` to either `AZURE_RTOS` or `FREERTOS` depending on which middleware is appropriate for your project:

    ```cmake
    set(VT_MIDDLEWARE_RTOS "AZURE_RTOS")
    ```
1. In your CMake, add the VT directory:

    ```cmake
    add_subdirectory(verified-telemetry)
    ```
1. In your CMake, add VT into your targets link library:

    ```cmake
    target_link_libraries(${PROJECT_NAME}
        az::iot::vt
    )
    ```

## Porting to your Platform

Verified Telemetry requires platform specific functions to be defined at the application layer and then passed to VT during initialization.

> **NOTE**
> * Refer to [vt_device_driver.h](./inc/platform/vt_device_driver.h) for function declarations
> * Refer to [vt_device_driver_template.c](./templates/vt_device_driver_template.c) for a template for implementation of the platform functions.
> * Samples for implementation of platform functions can also be found in the [Device samples](https://github.com/Azure/Verified-Telemetry-Device-Sample/blob/main/MXChip/AZ3166/app/sample_vt_device_driver.c).

The application developer will implement the following functions:

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

## Support

If you need support, please see our [SUPPORT.md](./SUPPORT.md) file.

## Contributing

For details on contributing to this repository, see the [contributing](CONTRIBUTING.md) guide.

## Reporting Security Vulnerabilities

If you believe you have found a security vulnerability in any Microsoft-owned repository that meets Microsoft's definition of a security vulnerability, please report it to the [Microsoft Security Response Center](SECURITY.md).

## License

Verified Telemetry are licensed under the [MIT](LICENSE.txt) license.
