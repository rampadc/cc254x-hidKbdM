# Overview

This is a firmware for Texas Instruments CC254x Bluetooth Low Energy microcontroller that sends
keyboard and mouse reports via Bluetooth to the host. The commands to control the reports are sent
via UART at 9600 bps with 8 data bits and no parity bit.

The device operates in 2 modes: command mode and translation mode.

 - If 3 consequence @'s, ie. @@@ is sent, put device into command mode
 - If 3 consequent $'s, ie. $$$ is sent, put device into translation mode

In command mode, the device parse commands sent by the host that are followed by a CR or LF character. The list of commands implemented is listed below:
+ KU[value]: a key is released with an 8-bit value that corresponds to a HID keycode
+ KD[value]: a key is pressed with an 8-bit value that corresponds to a HID keycode
+ M[status][X][Y][Z]: sending mouse buttons and mouse coordinates
+ KUPDATE: send report to host
+ S,R: reset the device
+ S,DC: disconnect from host
+ SN,[value]: change scan response data's string and device name. [value] must be less than 20 characters long. Restart device to see changes.

In translation mode, the device automatically translate ASCII characters to HID keycodes and emulate the key being pressed and released. The characters being covered is currently limited to the first 128 ASCII characters. Other keys like F1-F12, PrintScreen, etc. can only be sent in command mode.

Pre-built firmwares are included as HEX files in the bin folder in CC2540DB and CC2541DB. Since most HM-10 modules now use CC2541, firmware compilation for CC2540 has since been stopped. However, since they both share the same code base, you can compile the firmware yourself. The firmware compiled includes:
+ HM-10_CC254x_DisplayOnly.hex: Early release, to-be-updated soon. (5/8/2014) - obsolete
+ HM-10_CC254x_KeyboardOnly.hex: Early release, to be updated soon (5/8/2014) - obsolete
+ HM-10_CC2541_Experimental.hex: Firmware that supports command mode and translation mode. Passcode 000000. (23/12/2014)

**Features to be implemented**
+ Receive LED reports from host and send it over UART to external MCU

The development for this project is potentially ceased. 

# Setup
This was developed with IAR Embedded Workbench for 8051 v8.30 and BLE-stack v1.4.0. It is not updated to the latest BLE-stack v1.4.1.

To flash the firmware onto the chip, CC Debugger ($49) from Texas Instruments need to be used with 
SmartRF Flash Programmer (free). Connection to HM-10 is as follows:

![](https://flashandrc.files.wordpress.com/2014/08/connection.png)

To customize the firmare, IAR Embedded Workspace for 8051 is needed. You may want to add:
    #include "bcomdef.h"
    #include "gatt.h"
into hiddev.h to correct some compilation errors, as shown in the photo below:

![](https://flashandrc.files.wordpress.com/2014/10/hiddev_adding_includes.png)

The source code included needs to be copied into a folder created inside TI's official BLE API 1.4's
Project folder as the INCLUDES defined in the Preprocessor are set up for this. The official API was 
not included in the project as the license agreement does not allow this.

![](https://flashandrc.files.wordpress.com/2014/10/folder-struct.png)

# License

Copyright 2014 Cong Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
