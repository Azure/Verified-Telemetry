# API Documentation
## Overview of functions
| Function          | Description                                                                     |
|-----------------|---------------------------------------------------------------------------------|
| [pnp_vt_init](./VT_API.md#pnp_vt_init) | Initializes Global Verified Telemetry                |
| [pnp_fallcurve_init](./VT_API.md#pnp_fallcurve_init)  | Initializes an instance of Verified Telemetry Information Interface                   |
| [pnp_vt_process_command](./VT_API.md#pnp_vt_process_command)  | Processes all commands supported by vT Middleware                 |
| [pnp_vt_process_property_update](./VT_API.md#pnp_vt_process_property_update)  |  Processes all desired property updates supported by vT Middleware               |
| [pnp_vt_process_reported_property_sync](./VT_API.md#pnp_vt_process_reported_property_sync)  |  Syncronizes vT Settings stored in digital Twin as reported properties at startup               |
| [pnp_vt_properties](./VT_API.md#pnp_vt_properties)  | Creates payloads and sends all reported properties supported by vT Middleware                 |
 
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
    GPIO_PORT_TYPEDEF *                 gpio_port,
    GPIO_PIN_TYPEDEF               	gpio_pin, 
    ADC_CONTROLLER_TYPEDEF *            adc_controller,
    ADC_CHANNEL_TYPEDEF                 adc_channel,
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

* `gpio_port` GPIO Port to which the corresponding sensor is connected.

* `gpio_pin` GPIO Pin to which the corresponding sensor is connected.

* `adc_controller` ADC Controller to which the corresponding sensor is connected.

* `adc_channel` ADC Channel to which the corresponding sensor is connected.

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