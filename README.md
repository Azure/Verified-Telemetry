# Verified Telemetry Overview
The quality of sensor data is fundamental for IoT adoption. Since IoT sensors are made of low-cost components, deployed in-the-wild and in harsh environments, they are prone to failures leading to faulty or dirty data. The effect of bad data will lead to the well-known garbage-in garbage-out process, leading to cascading ill-effects upstream and at times catastrophic decisions.  

Verified Telemetry (VT) is a state-of-the-art solution to determine the health of the sensor, i.e., working or faulty, which is consequently used to determine the quality of the sensed data. This is achieved by devising an intelligent “sensor fingerprint”, a set of unique electrical characteristics that differs between working and faulty sensors. The fingerprints can detect faults for a wide variety of off-the-shelf sensors and can be easily implemented with lightweight software code running on the IoT device. This novel approach empowers customers with a reliable and automated way to remotely measure and observe the health of the sensor in real-time alongside the data collected. The data associated with a validated fingerprint results in verified telemetry (as shown below).

![VT_Overview](./docs/Azure_VT.PNG)

# Table of Contents

* [Architecture](https://github.com/Azure/Verified-Telemetry#architecture)
* [File Structure](https://github.com/Azure/Verified-Telemetry#file-structure)
* [Dependencies](https://github.com/Azure/Verified-Telemetry#dependencies)
* [Samples](https://github.com/Azure/Verified-Telemetry#samples)
	* [Device Samples](https://github.com/Azure/Verified-Telemetry#device-sample)
	* [Solution Samples](https://github.com/Azure/Verified-Telemetry#solution-sample)
* [Resource Requirements](https://github.com/Azure/Verified-Telemetry#resource-requirements)
* [Plug and Play Model for VT](https://github.com/Azure/Verified-Telemetry#plug-and-play-model)
* [API Documentation](https://github.com/Azure/Verified-Telemetry#api-documentation)
* [Integration of VT with existing device code](https://github.com/Azure/Verified-Telemetry#integration-of-VT-with-existing-device-code)


# Architecture
Verified Telemetry Library currently supports Azure RTOS and uses the following components:
* [Azure NetX Duo.](https://github.com/azure-rtos/netxduo) Provides a full TCP/IP IPv4 and IPv6 network stack, and networking support integrated with ThreadX.
* [Azure IoT Middleware for Azure RTOS](https://github.com/azure-rtos/netxduo/tree/master/addons/azure_iot) Platform specific library that acts as a binding layer between the Azure RTOS and the Azure SDK for Embedded C.

The architecture of VT library is also shown below:
![Architecture](./docs/architecture.png)


# File Structure
The VT library has been structured around the following components:
* [Core.](./src/core) This module includes core functions to support verified telemetry, such as functions to collect, validate and evaluate sensor fingerprints.
* [Middleware.](./src/middleware) This module contains implementations to support interactions with Azure IoT Middleware for Azure RTOS.
* [Platform.](./src/platform) This module helps in making the library compatible with several hardware devices and OS kernels.

The structure of the library is as follows:
* **/inc** Contains Header files for core, middeware and platform components

* **/src** Contains implementations for all the header files in **/inc**.

* **/PnPModel** Contains impementations for sample PnP models to support Verified Telemtery.

* **CmakeLists.txt**  
	CMake script to generate buid files.	
	Verified Telemetry provides several options to customize the library according to the project requirement:

	| Option          | Description                                                                     | Available Values                                  | Default Value				  |
	|-----------------|---------------------------------------------------------------------------------|---------------------------------------------------|-----------------------------|
	| `VT_DEVICE`  | This option links the correct HAL implementation to the library.                | `MXCHIP_AZ3166` `STM_BL475EIOT01A` `CUSTOM_DEVICE`| `MXCHIP_AZ3166`			  |

	
	Note: While selecting `CUSTOM_DEVICE`  option, user must provide implementations for the functions present [vt_dsc_custom.c](./src/platform/vt_dsc_custom.c).

# Dependencies
Verified Telemetry is dependant on following SDK's:
  * [NetXDuo](https://github.com/azure-rtos/netxduo/tree/feature/iot_pnp)
  * Device Specific HAL SDK (as shown in the table below)

    | Device          | SDK                                                           |																				  
    |-----------------|---------------------------------------------------------------|
    | [MXChip_AZ3166](https://en.mxchip.com/az3166)									  | [STM32Cubef4](https://github.com/STMicroelectronics/STM32CubeF4) |
    | [B-L475E-IOT01A](https://www.st.com/en/evaluation-tools/b-l475e-iot01a.html)	  | [STM32Cubel4](https://github.com/STMicroelectronics/STM32CubeL4)|



# Samples
We provide multiple device and solution sample to showcase the usage of verified Telemetry. Please follow the following Getting started Guides to get started. 
## Device Samples 
These [device sample](https://github.com/Azure/Verified-Telemetry-Device-Sample) shows device developers how to include Verified Telemetry with Azure IoT on Azure RTOS.

## Solution Samples 
These [solution samples](https://github.com/Azure/Verified-Telemetry-Solution-Sample) showcase how the Verified Telemetry features can be utilised in real world scenarios. This uses InfluxDB, Grafana and the [Azure IoT Node.js SDK](https://github.com/Azure/azure-iot-sdk-node) to communicate with Azure IoT Hub.

# Resource Requirements
We except Verified Telemetery code to run on microcontrollers, which have very limited amounts of flash and RAM. Below is our resource requirements to add verified telemetry code:

| `Flash`       |  46 Kb  |																				  
|---------------|---------|
| `RAM`		    | 14 Kb   |

# Plug and Play Model
Verified Telemetry Library provides capabilities for interaction using a Plug and Play Model.
Details about this model can be found [here](./PnPModel).

# API Documentation
| Function          | Description                                                                     |
|-----------------|---------------------------------------------------------------------------------|
| [pnp_vt_init](./docs/VT_API.md#pnp_vt_init) | Initializes Global Verified Telemetry                |
| [pnp_fallcurve_init](./docs/VT_API.md#pnp_fallcurve_init)  | Initializes an instance of Verified Telemetry Information Interface                   |
| [pnp_vt_process_command](./docs/VT_API.md#pnp_vt_process_command)  | Processes all commands supported by vT Middleware                 |
| [pnp_vt_process_property_update](./docs/VT_API.md#pnp_vt_process_property_update)  |  Processes all desired property updates supported by vT Middleware               |
| [pnp_vt_process_reported_property_sync](./docs/VT_API.md#pnp_vt_process_reported_property_sync)  |  Syncronizes vT Settings stored in digital Twin as reported properties at startup               |
| [pnp_vt_properties](./docs/VT_API.md#pnp_vt_properties)  | Creates payloads and sends all reported properties supported by vT Middleware                 |
 

# Integration of VT with existing device code
* Developers can refer to the [Device Samples](https://github.com/Azure/Verified-Telemetry-Device-Sample) for Verified Telemetry sample code. The following steps showcase how Verified Telemetry Library can be integrated separately with [Azure RTOS getting started samples](https://github.com/azure-rtos/getting-started)

	> NOTE: The following steps demonstrate the integration of Verified Telemetry for the [AZ3166 getting started sample.](https://github.com/azure-rtos/getting-started/tree/master/MXChip/AZ3166) 
	Similar steps can be followed for B-L475E-IOT01A/B-L4S5I-IOT01A device samples.

## Step 1: Include Verified Telemetry library
* In the root folder, run the following command
	```shell
	git submodule add https://github.com/Azure/Verified-Telemetry.git core/lib/verified-telemetry
	```
* Include library files in [getting-started/MXChip/AZ3166/lib/CMakeLists.txt](https://github.com/azure-rtos/getting-started/blob/master/MXChip/AZ3166/lib/CMakeLists.txt)
	```
	add_subdirectory(${CORE_LIB_DIR}/verified-telemetry verified_telemetry)
	```
* Configure VT_DEVICE option in [getting-started/MXChip/AZ3166/lib/CMakeLists.txt](https://github.com/azure-rtos/getting-started/blob/master/MXChip/AZ3166/lib/CMakeLists.txt)
	```
	set(VT_DEVICE "MXCHIP_AZ3166")
	```
## Step 2: Initialize Verified Telemetry
Make the following additions to the [/getting-started/blob/master/MXChip/AZ3166/app/nx_client.c](https://github.com/azure-rtos/getting-started/blob/master/MXChip/AZ3166/app/nx_client.c) file
* Include headers
	```C
	#include "pnp_verified_telemetry.h"
	#include "pnp_fallcurve_component.h"
	```
* Define number of verified Telemetries that the device would be supporting
	```C
	#define NUMBER_OF_VT_ENABLED_TELEMETRIES                         N
	```
* Define Verified Telemetry DB which will store configuration and runtime information
	```C
	static VERIFIED_TELEMETRY_DB verified_telemetry_DB;
	```
* Define one FallCurve component for each telemetry that will be supporting Verified Telemetry
	```C
	static PNP_FALLCURVE_COMPONENT sample_fallcurve_1;
	// static PNP_FALLCURVE_COMPONENT sample_fallcurve_2;
	static PNP_FALLCURVE_COMPONENT sample_fallcurve_N;
	```
* Assign a name to each FallCurve component
	```C
	static const CHAR sample_fallcurve_1_component[] = "vTTelemetry1";
	// static const CHAR sample_fallcurve_2_component[] = "vTTelemetry2";
	static const CHAR sample_fallcurve_N_component[] = "vTTelemetryN";
	```
* Register Verified Telemetry components with Azure RTOS Middleware for Azure IoT
 	```C
	nx_azure_iot_pnp_client_component_add( iotpnp_client_ptr, 
					       (const UCHAR *)sample_fallcurve_1_component,
           				       sizeof(sample_fallcurve_1_component) - 1));
	// nx_azure_iot_pnp_client_component_add( iotpnp_client_ptr, 
					        (const UCHAR *)sample_fallcurve_2_component,
           				        sizeof(sample_fallcurve_2_component) - 1));
	nx_azure_iot_pnp_client_component_add( iotpnp_client_ptr, 
					       (const UCHAR *)sample_fallcurve_N_component,
           				       sizeof(sample_fallcurve_N_component) - 1))				       
					       
	```
* Define variables to hold Verified Telemetry configuration information
	```C
	static PNP_FALLCURVE_COMPONENT * fallcurve_components[NUMBER_OF_VT_ENABLED_TELEMETRIES] = {NULL};
	static CHAR * connected_sensors[NUMBER_OF_VT_ENABLED_TELEMETRIES] = {NULL};
	```
* Define a Verified Telemetry User Initialization wrapper function which will make calls to necessary initilization function from the vT library
	```C
	void * sample_pnp_verified_telemetry_user_init()
	{
	    UINT status;
	    if ((status = pnp_fallcurve_init(&sample_fallcurve_1, (UCHAR *)"vTTelemetry1", GPIOB,
						    GPIO_PIN_8, &hadc1, ADC_CHANNEL_1, NULL, fallcurve_components,
						    connected_sensors, (UCHAR *) "Telemetry1", 
						    NUMBER_OF_VT_ENABLED_TELEMETRIES)))
	    {
		printf("Failed to initialize vTTelemetry1 component: error code = 0x%08x\r\n", status);
	    }
	    
    	//  else if ((status = pnp_fallcurve_init(&sample_fallcurve_2, (UCHAR *)"vTTelemetry2", GPIOB,
		// 					    GPIO_PIN_10, &hadc1, ADC_CHANNEL_3, NULL, fallcurve_components,
		// 					    connected_sensors, (UCHAR *) "Telemetry2", 
		// 					    NUMBER_OF_VT_ENABLED_TELEMETRIES)))
    	// {
    	//     printf("Failed to initialize vTTelemetry2 component: error code = 0x%08x\r\n", status);
    	// }
		
	    else if ((status = pnp_fallcurve_init(&sample_fallcurve_N, (UCHAR *) "vTTelemetryN", GPIOB,
						    GPIO_PIN_9, &hadc1, ADC_CHANNEL_2, NULL, fallcurve_components,
						    connected_sensors, (UCHAR *) "TelemetryN", 
						    NUMBER_OF_VT_ENABLED_TELEMETRIES )))
	    {
		printf("Failed to initialize vTTelemetryN component: error code = 0x%08x\r\n", status);
	    }
	    else if ((status = pnp_vt_init(&verified_telemetry_DB, fallcurve_components, NUMBER_OF_VT_ENABLED_TELEMETRIES, 
							true, 0x080E0000 )))
	    {
		printf("Failed to initialize Verified Telemetry: error code = 0x%08x\r\n", status);
	    }
	    return (void*)(&verified_telemetry_DB);
	}
	```
* Call the Verified Telemetry User Initialization wrapper function and update verified_telemetry_DB
	 ```C
	verified_telemetry_DB = sample_pnp_verified_telemetry_user_init();
	 ```
## Step 3: Add Verified Telemetry functions to existing routines for Property updates and Command Processing
Make the following modifications to the [/getting-started/blob/master/MXChip/AZ3166/app/nx_client.c](https://github.com/azure-rtos/getting-started/blob/master/MXChip/AZ3166/app/nx_client.c) file

* Invoke Verified Telemetry Property transmission function at regular intervals
	```C
	pnp_vt_properties(verified_telemetry_DB, &(context->iotpnp_client));
	```
* Call Verified Telemetry Command Processing when direct methods/ commands are received
	```C
	pnp_vt_process_command( verified_telemetry_DB, &(sample_context_ptr->iotpnp_client),
                 		(UCHAR *)component_name_ptr, component_name_length,
                  		(UCHAR *)pnp_command_name_ptr, pnp_command_name_length,
                  		&json_reader, &json_writer, &status_code);
	```
