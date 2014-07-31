/* 
 * File name: hidKbdMouse.h
 * Description: Implementation of the application
 *
 * Copyright 2014 Cong Nguyen
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, 
 * software distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and limitations under the License. 
*/

#ifndef HIDKBDMOUSE_H
#define HIDKBDMOUSE_H

#ifdef __cplusplus
extern "C"
{
#endif
  
  /*********************************************************************
  * INCLUDES
  */
  
  /*********************************************************************
  * CONSTANTS
  */
  
  // Task Events
#define START_DEVICE_EVT                              0x0001
#define SBP_PERIODIC_EVT                                  0x0002
  
  /*********************************************************************
  * MACROS
  */ 
  
  typedef enum {
    SERIAL_STATE_START = 0,
    SERIAL_STATE_TYPE,
    SERIAL_STATE_LEN,
    SERIAL_STATE_DATA,
    SERIAL_STATE_COMPLETE //received complete serial message
  } serial_state_t;
  
  /*********************************************************************
  * FUNCTIONS
  */
  
  extern void printf(char *format, ...);
  /*********************************************************************
  * GLOBAL VARIABLES
  */
  
  /*
  * Task Initialization for the BLE Application
  */
  extern void HidKbdMouse_Init( uint8 task_id );
  
  /*
  * Task Event Processor for the BLE Application
  */
  extern uint16 HidKbdMouse_ProcessEvent( uint8 task_id, uint16 events );
  
  /*********************************************************************
  *********************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* HIDKBDMOUSE_H */
