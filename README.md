cc2540-hidKbdM
==============

# Overview
This is a firmware for Texas Instruments CC2540 Bluetooth Low Energy microcontroller that sends
keyboard and mouse reports via Bluetooth to the host. The commands to control the reports are sent
via UART at 57600 bps with 8 data bits and no parity bit.

These includes:
+ KU(keycode): a key is released with the keycode (keycode)
+ KD(keycode): a key is pressed with the keycode (keycode)
+ M(status)(X)(Y)(Z): sending mouse buttons and mouse coordinates
+ KUPDATE: send report to host
(___) is an 8-byte character, in which only the value inside the brackets are needed (not including
brackets)

Pre-built firmwares are included as HEX files in the bin folder. These includes builds for TI CC2540
keyfob, HM-10 variant that uses CC2540 (to be tested soon)

# Setup
To flash the firmware onto the chip, CC Debugger ($49) from Texas Instruments need to be used with 
SmartRF Flash Programmer (free). 

The source code included needs to be copied into a folder created inside TI's official BLE API 1.4's
Project folder as the INCLUDES defined in the Preprocessor are set up for this. The official API was 
not included in the project as the license agreement does not allow this.

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
