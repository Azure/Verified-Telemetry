# Model File Description
## Verified Telemetry Information Interface
The main interface of Verified Telemetry is [Verified Telemetry Information](./vTInfo.json).

| Type | Name | Description |
|---|---|---|
| **Properties (read-only)** | `telemetryStatus` | Status of the telemetry, i.e. Working/Faulty to which the component of this interface is asscoiated. |
| **Properties (read-only)** | `fingerprintType` | Type of the fingerprint (String). e.g., FallCurve or CurrentSense or Custom. |
| **Properties (read-only)** | `fingerprintTemplate` | Template Fingerprint information in a Map |
| **Commands** | `resetFingerprintTemplate` | This command will reset the template fingerprint |

## [Getting started guide](./sample/gsg.json) interface has the following components:
1. Device Component using the [Device Sensors](./sample/device.json) interface. This represents the Default Device Component from the ODMs.
1. vTsoilMoistureExternal Component using the [Verified Telemetry Information](./vTInfo.json). This represents the verified telemetry component for soilMoistureExternal telemetry.
1. vTaccelerometerXExternal Component using the [Verified Telemetry Information](./vTInfo.json). This represents the verified telemetry component for accelerometerXExternal telemetry.


| Type | Name | Description |
|---|---|---|
| **Properties (read-only)** | `deviceStatus` | Overall Device Status (Working or Faulty). e.g., A device status is set to Faulty if any of the sensor has a fault. |
| **Properties (writable)** | `enableVerifiedTelemetry` | Enable Verified Telemetry (True/False). e.g., True enables Verified Telemetry functionalities. |
| **Interface** | `sampleDevice` | Default Device Component from the ODM. |
| **Interface** | `vTsoilMoistureExternal` |  The Verified Telemetry component for soilMoistureExternal telemetry. |
| **Interface** | `vTaccelerometerXExternal` | The Verified Telemetry component for accelerometerXExternal telemetry. |