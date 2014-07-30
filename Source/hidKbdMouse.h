/**************************************************************************************************
Filename:       hidKbdMouse.h

Description:    This file contains the HID keyboard & mouse application
definitions and prototypes.

**************************************************************************************************/

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
