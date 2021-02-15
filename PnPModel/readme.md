# Model File 
We now provide the details of the PnP model files requried to support Verified Telemetry. 

## Verified Telemetry Information Interface
The main interface of Verified Telemetry is defined in [Verified Telemetry Information](./vTInfo.json). This interface includes all the properties and commands to support Verified Telemetry. 

| Type | Name | Description |
|---|---|---|
| **Properties (read-only)** | `telemetryStatus` | Status of the telemetry, i.e. Working/Faulty to which the component of this interface is asscoiated. |
| **Properties (read-only)** | `fingerprintType` | Type of the fingerprint (String). e.g., FallCurve or CurrentSense or Custom. |
| **Properties (read-only)** | `fingerprintTemplate` | Template Fingerprint information of a working sensor. |
| **Commands** | `resetFingerprintTemplate` | This command will clear the template fingerprint of a sensor and collect a new template fingerprint of the attached sensor. |
| **Commands** | `retrainFingerprintTemplate` | This command will append a new template fingerprint to the `fingerprintTemplate'. Appending more fingerprints will increase the accuracy in detecting the telemetry status. |

## Getting started sample model
The [Getting started guide](./sample/gsg.json) interface has the following components:
1. Device Component uses the [Device Sensors](./sample/device.json) interface. This represents the default Device Component from the manufacturers.
1. vTsoilMoistureExternal Component uses the [Verified Telemetry Information](./vTInfo.json). This represents the verified telemetry component for soilMoistureExternal telemetry.
1. vTaccelerometerXExternal Component uses the [Verified Telemetry Information](./vTInfo.json). This represents the verified telemetry component for accelerometerXExternal telemetry.

	> NOTE: Include as many components with [Verified Telemetry Information](./vTInfo.json) as there are telemetries for which verifcation is required. In the above example Verified Telemetry is enabled on two sensors SoilMositure and AccelerometerX.

The structure of the PnP model for Getting Started Guide is as follows:

| Type | Name | Description |
|---|---|---|
| **Properties (read-only)** | `deviceStatus` | Overall Device Status (Working or Faulty). e.g., A device status is set to Faulty if any of the sensor has a fault. |
| **Properties (writable)** | `enableVerifiedTelemetry` | Enable Verified Telemetry (True/False). e.g., True enables Verified Telemetry functionalities. |
| **Interface** | `sampleDevice` | Default Device Component from the manufacturer. |
| **Interface** | `vTsoilMoistureExternal` |  The Verified Telemetry component for soilMoistureExternal telemetry. |
| **Interface** | `vTaccelerometerXExternal` | The Verified Telemetry component for accelerometerXExternal telemetry. |