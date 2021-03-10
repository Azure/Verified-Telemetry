# Model File 
We now provide the details of the PnP model files requried to support Verified Telemetry. 

## Verified Telemetry Information Interface
The main interface of Verified Telemetry is defined in [Verified Telemetry Information](./vTInfo.json). This interface includes all the properties and commands to support Verified Telemetry. 

| Type | Name | Description |
|---|---|---|
| **Properties (read-only)** | `telemetryStatus` | Status of the telemetry, i.e. Working/Faulty to which the component of this interface is asscoiated. |
| **Properties (read-only)** | `fingerprintType` | Type of the fingerprint (String). e.g., FallCurve or CurrentSense or Custom. |
| **Properties (read-only)** | `fingerprintTemplate` | Template Fingerprint information of a working sensor. |
| **Properties (read-only)** | `fingerprintTemplateConfidenceMetric` | Stores information on how much the Fingerprint Template can be trusted |
| **Commands** | `setResetFingerprintTemplate` | This command will clear the template fingerprint of a sensor and collect a new template fingerprint of the attached sensor. |
| **Commands** | `retrainFingerprintTemplate` | This command will append a new template fingerprint to the `fingerprintTemplate'. Appending more fingerprints will increase the accuracy in detecting the telemetry status. |

## Verified Telemetry Device Interface
* The [Verified Telemetry Device Information](./vTDevice.json) Interface implements and conveys device wide Verified Telemetry Settings and Information 
    | Type | Name | Description |
    |---|---|---|
    | **Properties (writable)** | `enableVerifiedTelemetry` | Controls whether Fingerprint Collection and Evaluation is implemented or not. When this property is set to 'false', Telemetry Verification cannot be performed.  |
    | **Properties (read-only)** | `deviceStatus` | Device status is set to false if any sensor supported by VT has a fault. |

## Getting started sample model
The [Getting started guide](./sample/gsg.json) interface has the following components:
1. Device Component uses the [Device Sensors](./sample/device.json) interface. This represents the default Device Component from the manufacturers.
1. vTsoilMoistureExternal1 Component uses the [Verified Telemetry Information](./vTInfo.json) interface. This represents the verified telemetry component for soilMoistureExternal1 telemetry.
1. vTsoilMoistureExternal2 Component uses the [Verified Telemetry Information](./vTInfo.json) interface. This represents the verified telemetry component for soilMoistureExternal2 telemetry.
	> NOTE: Include as many components with [Verified Telemetry Information](./vTInfo.json) as there are telemetries for which verifcation is required. In the above example Verified Telemetry is enabled on two sensors Soil Moisture 1 and Soil Moisture 2.

1. vTDevice Component uses the [Verified Telemetry Device Information](./vTDevice.json) interface.



The structure of the PnP model for [Getting started guide](./sample/gsg.json) is as follows:

| Type | Name | Description | Interface ID |
|---|---|---|---|
| **Component** | `sampleDevice` | Default Device Component from the manufacturer. | dtmi:azure:verifiedtelemetry:sample:GSG;1 |
| **Component** | `vTDevice` | Device Level Verified Telemetry component | dtmi:azure:verifiedtelemetry:deviceinformation;1 | 
| **Component** | `vTsoilMoistureExternal1` | The Verified Telemetry component for soilMoistureExternal1 telemetry. | dtmi:azure:verifiedtelemetry:telemetryinformation;1 | 
| **Component** | `vTsoilMoistureExternal2` | The Verified Telemetry component for soilMoistureExternal2 telemetry. | dtmi:azure:verifiedtelemetry:telemetryinformation;1 | 