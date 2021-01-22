# Verified Telemetry
- Verified Telemetry is a state-of-the-art solution to determine the working of the sensor, i.e., working or faulty, consequently used to determine the quality of the sensed data. 
- This is achieved by devising an intelligent fingerprint for a sensor to determine the status of the sensor.  
- The sensor fingerprint captures electrical properties of the sensor such as voltage and current using seamless software code running on the IoT device. 

# Architecture Diagram
![Architecture](./docs/architecture.png)

#Dependencies
Verified Telemetry is dependant on following SDK's:
  * [NetXDuo](https://github.com/azure-rtos/netxduo)
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



