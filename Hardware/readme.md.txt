# PCB files for Verified Telemetry Hardware 

Includes Gerber, Assembly, Schematic, Board and BOM files for manufacturing of hardware required to setup Verified Telemetry on MX Chip, STM32 and ESP32. 
The hardware contains CS baseboards for differet MCUs and CS hat.

Baseboards are MCU specific and are used to route the right pins on the MCU to the CS hat for VT functioning. These are 

CS hat contains the components required to sample signals and communicate them to the MCU using SPI. CS hat gets fixed on the baseboard and no other wiring is required to setup the hardware. 


## ST Microelectronics B-L475E-IOT01A / B-L4S5I-IOTOA1 Discovery kit Baseboard

![stm_baseboard](media/stm_baseboard.png)

## MXChip AZ3166 IoT DevKit Baseboard

![mxc_baseboard](media/mxc_baseboard.png)

## ESP 32 B-L475E-IOT01A / B-L4S5I-IOTOA1 Discovery kit Baseboard

![esp_baseboard](media/esp_baseboard.png)

## CurrentSense (CS) Hat

![cs_hat](media/cshat.png)

![stm_hat](media/stm_hat.png)

![mxc_hat](media/mxc_hat.png)

![esp_hat](media/esp_hat.png)