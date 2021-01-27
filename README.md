# Verified Telemetry
- Verified Telemetry is a state-of-the-art solution to determine the working of the sensor, i.e., working or faulty, consequently used to determine the quality of the sensed data. 
- This is achieved by devising an intelligent fingerprint for a sensor to determine the status of the sensor.  
- The sensor fingerprint captures electrical properties of the sensor such as voltage and current using seamless software code running on the IoT device. 

# [Verified Telemetry Device Samples](https://github.com/Azure/Verified-Telemetry-Device-Sample)
- These Getting Started guides shows device developers how to include Verified Telemetry with Azure IoT on Azure RTOS.

# [Verified Telemetry Solution Samples](https://github.com/Azure/Verified-Telemetry-Solution-Sample)
- These Getting Started guides showcase how the Verified Telemetry features can be utilised in real world scenarios.


# Architecture Diagram
![Architecture](./docs/architecture.png)

# Dependencies
Verified Telemetry is dependant on following SDK's:
  * [NetXDuo](https://github.com/azure-rtos/netxduo/tree/feature/iot_pnp)
  * Device Specific HAL SDK (as shown in the table below)

    | Device          | SDK                                                           |																				  
    |-----------------|---------------------------------------------------------------|
    | [MXChip_AZ3166](https://en.mxchip.com/az3166)									  | [STM32Cubef4](https://github.com/STMicroelectronics/STM32CubeF4) |
    | [B-L475E-IOT01A](https://www.st.com/en/evaluation-tools/b-l475e-iot01a.html)	  | [STM32Cubel4](https://github.com/STMicroelectronics/STM32CubeL4)|
      


# File Structure

* **/inc**  
	Contains Header files
	* **/core**  
		This module includes core functions to support verified telemetry, such as functions to collect, validate and evaluate sensor fingerprints.	
	* **/middleware**  
		This module contains implementations to support interactions with Azure IoT services (IoT hub, Central, etc.). The Azure IoT interactions are described in terms of telemetry, properties, and commands using the Digital Twins Definition Language (DTDL) version 2. 

	* **/platform**  
		This module helps in making the library compatible with several hardware devices and OS kernels.

* **/src**  
	Contains implementations for all the header files mentioned above.

* **CmakeLists.txt**  
	CMake script to generate buid files.	
	Verified Telemetry provides several options to customize the library according to project requirement:

	| Option          | Description                                                                     | Available Values                                  | Default Value				  |
	|-----------------|---------------------------------------------------------------------------------|---------------------------------------------------|-----------------------------|
	| `VT_DEVICE`  | This option links the correct HAL implementation to the library.                | `MXCHIP_AZ3166` `STM_BL475EIOT01A` `CUSTOM_DEVICE`| `MXCHIP_AZ3166`			  |

	
	Note: While selecting `CUSTOM_DEVICE`  option, user must provide implementations for the functions present [vt_dsc_custom.c](./src/platform/vt_dsc_custom.c).


# Resource Requirements
| `Flash`       |  46 Kb  |																				  
|---------------|---------|
| `RAM`		    | 14 Kb   |

# Plug and Play Model
Verified Telemetry Library provides capabilities of interaction using a Plug and Play Model.
Details about this model can be found [here](./PnPModel)
# API Documentation
## Overview of functions
| Function          | Description                                                                     |
|-----------------|---------------------------------------------------------------------------------|
| `pnp_vt_init`  | Initializes Global Verified Telemetry                |
| `pnp_fallcurve_init`  | Initializes an instance of Verified Telemetry Information Interface                   |
| `pnp_vt_process_command`  | Processes all commands supported by vT Middleware                 |
| `pnp_vt_process_property_update`  |  Processes all desired property updates supported by vT Middleware               |
| `pnp_vt_process_reported_property_sync`  |  Syncronizes vT Settings stored in digital Twin as reported properties at startup               |
| `pnp_vt_properties`  | Creates payloads and sends all reported properties supported by vT Middleware                 |
 
# pnp_vt_init()

Initializes Global Verified Telemetry.

## Syntax

\#include "[verified-telemetry\inc\middleware\pnp_verified_telemetry.h](./inc/middleware/pnp_verified_telemetry.h)"  
```C
UINT pnp_vt_init(
	void * 				verified_telemetry_DB, 
	PNP_FALLCURVE_COMPONENT ** 	fallcurve_components,
	UINT 				numberVerifiedTelemetries,
	bool 				enableVerifiedTelemetry, 
	UINT 				flash_address
	);
```

## Parameters
* `verified_telemetry_DB` Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data. 

* `fallcurve_components` Pointer to array of variables of type PNP_FALLCURVE_COMPONENT holding information for each component of Verified Telemetry Information Interface. 

* `numberVerifiedTelemetries` Number of telemetries which would be supporting verified telemetry feature.

* `enableVerifiedTelemetry` User specified value to set Verified Telemetry active or inactive, can also be configured during runtime from a writable Digital Twin property.

* `flash_address` Starting address of FLASH memory where runtime configuration settings of Verified Telemetry can be stored. This can be NULL if user does not want to use FLASH to store Verified Telemetry runtime configuration settings.

## Return Value
NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.

# pnp_fallcurve_init()

Initializes an instance of Verified Telemetry Information Interface

## Syntax

\#include "[verified-telemetry\inc\middleware\pnp_fallcurve_component.h](./inc/middleware/pnp_fallcurve_component.h)"  
```C
UINT pnp_fallcurve_init(
    PNP_FALLCURVE_COMPONENT             handle, 
    UCHAR *                     	component_name_ptr,
    GPIO_PORT_TYPEDEF *                 GPIOx,
    GPION_PIN_TYPEDEF               	GPIO_Pin, 
    ADC_CONTROLLER_TYPEDEF *            ADC_Controller,
    ADC_CHANNEL_TYPEDEF                 ADC_Channel,
    TIMER_HANDLE_TYPEDEF *              Timer,
    PNP_FALLCURVE_COMPONENT **          fallcurve_components,
    CHAR **                     	connected_sensors,
    UCHAR *                     	associatedTelemetry
    UINT                        	numberVerifiedTelemetries
    );
```

## Parameters
* `handle` The handle created by a call to the initialization function.

* `component_name_ptr` Name of the component. 

* `GPIOx` GPIO Port to which the corresponding sensor is connected.

* `GPIO_Pin` GPIO Pin to which the corresponding sensor is connected.

* `ADC_Controller` ADC Controller to which the corresponding sensor is connected.

* `ADC_Channel` ADC Channel to which the corresponding sensor is connected.

* `Timer` Provide initialized timer for better performance of vT Library, if not available pass NULL.

* `fallcurve_components` Pointer to array of variables of type PNP_FALLCURVE_COMPONENT created by the function which holds information for each component of Verified Telemetry Information Interface.

* `connected_sensors` Pointer to list of connected sensor names created by the function.

* `associatedTelemetry` Name of the telemetry associated with this component.

* `numberVerifiedTelemetries` Number of telemetries which would be supporting verified telemetry feature.

## Return Value
NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.

# pnp_vt_process_command()

Processes all commands supported by vT Middleware

## Syntax

\#include "[verified-telemetry\inc\middleware\pnp_verified_telemetry.h](./inc/middleware/pnp_verified_telemetry.h)"  
```C
UINT pnp_vt_process_command(
    void *                              verified_telemetry_DB, 
    NX_AZURE_IOT_PNP_CLIENT *           iotpnp_client_ptr,
    UCHAR *                             component_name_ptr,
    UINT               	                component_name_length, 
    UCHAR *                             pnp_command_name_ptr,
    UINT                                pnp_command_name_length,
    NX_AZURE_IOT_JSON_READER *          json_reader_ptr,
    NX_AZURE_IOT_JSON_WRITER *          json_response_ptr,
    UINT *                     	        status_code
    );
```

## Parameters
* `verified_telemetry_DB` Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data. 

* `iotpnp_client_ptr` Pointer to initialized Azure IoT PnP instance.

* `component_name_ptr` Name of the component. 

* `component_name_length` Length of name of the component. 

* `pnp_command_name_ptr` Name of the command invoked.

* `pnp_command_name_length` Length of name of the command invoked.

* `json_reader_ptr` Pointer to read the JSON payload of command.

* `json_response_ptr` Pointer to write response JSON payload.

* `status_code` Status updated by function based on command execution.

## Return Value
NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.

# pnp_vt_process_property_update()

Processes all desired property updates supported by vT Middleware

## Syntax

\#include "[verified-telemetry\inc\middleware\pnp_verified_telemetry.h](./inc/middleware/pnp_verified_telemetry.h)"  
```C
UINT pnp_vt_process_property_update(
    void *                              verified_telemetry_DB, 
    NX_AZURE_IOT_PNP_CLIENT *           iotpnp_client_ptr,
    UCHAR *                             component_name_ptr,
    UINT               	                component_name_length, 
    NX_AZURE_IOT_JSON_READER *          name_value_reader_ptr,
    UINT                      	        version
    );
```

## Parameters
* `verified_telemetry_DB` Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data. 

* `iotpnp_client_ptr` Pointer to initialized Azure IoT PnP instance.

* `component_name_ptr` Name of the component. 

* `component_name_length` Length of name of the component. 

* `json_reader_ptr` Pointer to read the JSON payload of command.

* `version` Property version stored in digital twin.

## Return Value
NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.

# pnp_vt_process_reported_property_sync()

Syncronizes vT Settings stored in digital Twin as reported properties at startup

## Syntax

\#include "[verified-telemetry\inc\middleware\pnp_verified_telemetry.h](./inc/middleware/pnp_verified_telemetry.h)"  
```C
UINT pnp_vt_process_reported_property_sync(
    void *                              verified_telemetry_DB, 
    NX_AZURE_IOT_PNP_CLIENT *           iotpnp_client_ptr,
    UCHAR *                             component_name_ptr,
    UINT               	                component_name_length, 
    NX_AZURE_IOT_JSON_READER *          name_value_reader_ptr,
    UINT                      	        version
    );
```

## Parameters
* `verified_telemetry_DB` Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data. 

* `iotpnp_client_ptr` Pointer to initialized Azure IoT PnP instance.

* `component_name_ptr` Name of the component. 

* `component_name_length` Length of name of the component. 

* `json_reader_ptr` Pointer to read the JSON payload of command.

* `version` Property version stored in digital twin.

## Return Value
NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.

# pnp_vt_properties()

Creates payloads and sends all reported properties supported by vT Middleware 

## Syntax

\#include "[verified-telemetry\inc\middleware\pnp_verified_telemetry.h](./inc/middleware/pnp_verified_telemetry.h)"  
```C
UINT pnp_vt_properties(
    void *                              verified_telemetry_DB, 
    NX_AZURE_IOT_PNP_CLIENT *           iotpnp_client_ptr
    );
```

## Parameters
* `verified_telemetry_DB` Pointer to variable of type VERIFIED_TELEMETRY_DB storing Verified Telemetry data. 

* `iotpnp_client_ptr` Pointer to initialized Azure IoT PnP instance.

## Return Value
NX_AZURE_IOT_SUCCESS upon success or an error code upon failure.

# Steps to integrate Verified Telemetry with existing device code
* While developers can refer to the [Device Samples](https://github.com/Azure/Verified-Telemetry-Device-Sample) for Verified Telemetry, the following steps showcase how Verified Telemetry Library can be integrated separately with [Azure RTOS getting started samples](https://github.com/azure-rtos/getting-started)

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
