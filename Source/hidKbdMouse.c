/* 
 * File name: hidKbdMouse.c
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


/*********************************************************************
* INCLUDES
*/

#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "gatt.h"
#include "hci.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "gatt_profile_uuid.h"
#include "linkdb.h"
#include "peripheral.h"
#include "gapbondmgr.h"
#include "hidkbmservice.h"
#include "devinfoservice.h"
#include "battservice.h"
#include "hiddev.h"

#include "hidKbdMouse.h"
#include <string.h>

#include "KBD_Report.h"

/*********************************************************************
* MACROS
*/

//UART testing
#define SBP_PERIODIC_EVT_PERIOD         50

// Selected HID keycodes
#define KEY_RIGHT_ARROW             0x4F
#define KEY_LEFT_ARROW              0x50
#define KEY_NONE                    0x00

// Selected HID LED bitmaps
#define LED_NUM_LOCK                0x01
#define LED_CAPS_LOCK               0x02

// Selected HID mouse button values
#define MOUSE_BUTTON_1              0x01
#define MOUSE_BUTTON_NONE           0x00

// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN     8

// HID LED output report length
#define HID_LED_OUT_RPT_LEN         1

// HID mouse input report length
#define HID_MOUSE_IN_RPT_LEN        5

/*********************************************************************
* CONSTANTS
*/

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT              60000

// Minimum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         50

// Supervision timeout value (units of 10ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          500

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         10

// Default passcode
#define DEFAULT_PASSCODE                      0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_INITIATE

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_KEYBOARD_ONLY

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           6

/*********************************************************************
* TYPEDEFS
*/

/*********************************************************************
* GLOBAL VARIABLES
*/

// Task ID
uint8 hidKbdMouseTaskId;

/*********************************************************************
* EXTERNAL VARIABLES
*/

/*********************************************************************
* EXTERNAL FUNCTIONS
*/

/*********************************************************************
* LOCAL VARIABLES
*/

// GAP Profile - Name attribute for SCAN RSP data
static uint8 scanData[] =
{
  0x15,                             // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,   // AD Type = Complete local name
  'H',
  'I',
  'D',
  ' ',
  'K',
  'e',
  'y',
  'b',
  'o',
  'a',
  'r',
  'd',
  ' ',
  '&',
  ' ',
  'M',
  'o',
  'u',
  's',
  'e'
};

// Advertising data
static uint8 advData[] =
{
  // flags
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  
  // appearance
  0x03,   // length of this data
  GAP_ADTYPE_APPEARANCE,
  LO_UINT16(GAP_APPEARE_HID_KEYBOARD),
  HI_UINT16(GAP_APPEARE_HID_MOUSE),
  
  // service UUIDs
  0x05,   // length of this data
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(HID_SERV_UUID),
  HI_UINT16(HID_SERV_UUID),
  LO_UINT16(BATT_SERV_UUID),
  HI_UINT16(BATT_SERV_UUID)
};

// Device name attribute value
static CONST uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "HID Keyboard & Mouse";

// HID Dev configuration
static hidDevCfg_t hidKbdMouseCfg =
{
  DEFAULT_HID_IDLE_TIMEOUT,   // Idle timeout
  HID_KBD_FLAGS               // HID feature flags
};

// TRUE if boot mouse enabled
static uint8 hidBootMouseEnabled = FALSE;

/*********************************************************************
* LOCAL FUNCTIONS
*/

static void hidKbdMouse_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void hidKbdMouse_HandleKeys( uint8 shift, uint8 keys );
static void hidKbdMouseSendReport( uint8 keycode );
static void hidKbdMouseSendMouseReport(uint8 buttons, int8 dx, int8 dy, int8 dz);
static uint8 hidKbdMouseRcvReport( uint8 len, uint8 *pData );
static uint8 hidKbdMouseRptCB( uint8 id, uint8 type, uint16 uuid,
                              uint8 oper, uint8 *pLen, uint8 *pData );
static void hidKbdMouseEvtCB( uint8 evt );

// UART functions
static void setupUART(void);
static void uartCallback(uint8 port, uint8 event);
static void performPeriodicTask(void);

//Keyboard and mouse functions
static void processBuffer(void);

/*********************************************************************
* PROFILE CALLBACKS
*/

static hidDevCB_t hidKbdMouseHidCBs =
{
  hidKbdMouseRptCB,
  hidKbdMouseEvtCB,
  NULL
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*********************************************************************
* @fn      HidKbdMouse_Init
*
* @brief   Initialization function for the HidKbdMouse App Task.
*          This is called during initialization and should contain
*          any application specific initialization (ie. hardware
*          initialization/setup, table initialization, power up
*          notificaiton ... ).
*
* @param   task_id - the ID assigned by OSAL.  This ID should be
*                    used to send messages and set timers.
*
* @return  none
*/
void HidKbdMouse_Init( uint8 task_id )
{
  hidKbdMouseTaskId = task_id;
  
  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );
  
  // Setup the GAP Peripheral Role Profile
  {
    uint8 initial_advertising_enable = FALSE;
    
    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 0;
    
    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;
    
    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );
    
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advData ), advData );
    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanData ), scanData );
    
    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }
  
  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *) attDeviceName );
  
  // Setup the GAP Bond Manager
  {
    uint32 passkey = DEFAULT_PASSCODE;
    uint8 pairMode = DEFAULT_PAIRING_MODE;
    uint8 mitm = DEFAULT_MITM_MODE;
    uint8 ioCap = DEFAULT_IO_CAPABILITIES;
    uint8 bonding = DEFAULT_BONDING_MODE;
    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof( uint8 ), &bonding );
  }
  
  // Setup Battery Characteristic Values
  {
    uint8 critical = DEFAULT_BATT_CRITICAL_LEVEL;
    Batt_SetParameter( BATT_PARAM_CRITICAL_LEVEL, sizeof (uint8), &critical );
  }
  
  // Set up HID keyboard service
  HidKbM_AddService( );
  
  // Register for HID Dev callback
  HidDev_Register( &hidKbdMouseCfg, &hidKbdMouseHidCBs );
  
  // Register for all key events - This app will handle all key events
  RegisterForKeys( hidKbdMouseTaskId );
  
#if defined( CC2540_MINIDK )
  // makes sure LEDs are off
  HalLedSet( (HAL_LED_1 | HAL_LED_2), HAL_LED_MODE_OFF );
  
  // For keyfob board set GPIO pins into a power-optimized state
  // Note that there is still some leakage current from the buzzer,
  // accelerometer, LEDs, and buttons on the PCB.
  
  P0SEL = 0; // Configure Port 0 as GPIO
  P1SEL = 0; // Configure Port 1 as GPIO
  P2SEL = 0; // Configure Port 2 as GPIO
  
  P0DIR = 0xFC; // Port 0 pins P0.0 and P0.1 as input (buttons),
  // all others (P0.2-P0.7) as output
  P1DIR = 0xFF; // All port 1 pins (P1.0-P1.7) as output
  P2DIR = 0x1F; // All port 1 pins (P2.0-P2.4) as output
  
  P0 = 0x03; // All pins on port 0 to low except for P0.0 and P0.1 (buttons)
  P1 = 0;   // All pins on port 1 to low
  P2 = 0;   // All pins on port 2 to low
  
#endif // #if defined( CC2540_MINIDK )
  
  //init keyboard report manager
  KBD_Report_Init();
  
  // UART init
  setupUART();
  
  // Setup a delayed profile startup
  osal_set_event( hidKbdMouseTaskId, START_DEVICE_EVT );
}

/*********************************************************************
* @fn      HidKbdMouse_ProcessEvent
*
* @brief   HidKbdMouse Application Task event processor.  This function
*          include timers, messages and any other user defined events.
*          is called to process all events for the task.  Events
*
* @param   task_id  - The OSAL assigned task ID.
* @param   events - events to process.  This is a bit map and can
*                   contain more than one event.
*
* @return  events not processed
*/
uint16 HidKbdMouse_ProcessEvent( uint8 task_id, uint16 events )
{
  
  VOID task_id; // OSAL required parameter that isn't used in this function
  
  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;
    
    if ( (pMsg = osal_msg_receive( hidKbdMouseTaskId )) != NULL )
    {
      hidKbdMouse_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );
      
      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }
    
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  
  if ( events & START_DEVICE_EVT )
  {
    // Set timer for first periodic event
    osal_start_timerEx( hidKbdMouseTaskId, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    
    return ( events ^ START_DEVICE_EVT );
  }
  
  //Added to test UART  
  if ( events & SBP_PERIODIC_EVT )
  {
    // Restart timer
    if ( SBP_PERIODIC_EVT_PERIOD )
    {
      osal_start_timerEx( hidKbdMouseTaskId, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    }
    
    // Perform periodic application task
    performPeriodicTask();
    
    return (events ^ SBP_PERIODIC_EVT);
  }
  
  return 0;
}

/*********************************************************************
* @fn      hidKbdMouse_ProcessOSALMsg
*
* @brief   Process an incoming task message.
*
* @param   pMsg - message to process
*
* @return  none
*/
static void hidKbdMouse_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
  case KEY_CHANGE:
    hidKbdMouse_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
    break;
    
  default:
    break;
  }
}

/*********************************************************************
* @fn      hidKbdMouse_HandleKeys
*
* @brief   Handles all key events for this device.
*
* @param   shift - true if in shift/alt.
* @param   keys - bit field for key events. Valid entries:
*                 HAL_KEY_SW_2
*                 HAL_KEY_SW_1
*
* @return  none
*/
static void hidKbdMouse_HandleKeys( uint8 shift, uint8 keys )
{
  static uint8 prevKey1 = 0;
  static uint8 prevKey2 = 0;
  
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( (keys & HAL_KEY_SW_1) && (prevKey1 == 0) )
  {
    // pressed
    hidKbdMouseSendReport( KEY_LEFT_ARROW );
    prevKey1 = 1;
  }
  else if ( !(keys & HAL_KEY_SW_1) && (prevKey1 == 1) )
  {
    // released
    hidKbdMouseSendReport( KEY_NONE );
    prevKey1 = 0;
  }
  
  if ( (keys & HAL_KEY_SW_2) && (prevKey2 == 0) )
  {
    // pressed
    if ( !hidBootMouseEnabled )
    {
      hidKbdMouseSendReport( KEY_RIGHT_ARROW );
    }
    else
    {
     // hidKbdMouseSendMouseReport( MOUSE_BUTTON_1 );
    }
    prevKey2 = 1;
  }
  else if ( !(keys & HAL_KEY_SW_2) && (prevKey2 == 1) )
  {
    // released
    if ( !hidBootMouseEnabled )
    {
      hidKbdMouseSendReport( KEY_NONE );
    }
    else
    {
  //    hidKbdMouseSendMouseReport ( MOUSE_BUTTON_NONE );
    }
    prevKey2 = 0;
  }
}

/*********************************************************************
* @fn      hidKbdMouseSendReport
*
* @brief   Build and send a HID keyboard report.
*
* @param   keycode - HID keycode.
*
* @return  none
*/
static void hidKbdMouseSendReport( uint8 keycode )
{
  uint8 buf[HID_KEYBOARD_IN_RPT_LEN];
  
  buf[0] = 0;         // Modifier keys
  buf[1] = 0;         // Reserved
  buf[2] = keycode;   // Keycode 1
  buf[3] = 0;         // Keycode 2
  buf[4] = 0;         // Keycode 3
  buf[5] = 0;         // Keycode 4
  buf[6] = 0;         // Keycode 5
  buf[7] = 0;         // Keycode 6
  
  HidDev_Report( HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                HID_KEYBOARD_IN_RPT_LEN, buf );
}

/*********************************************************************
* @fn      hidKbdMouseSendMouseReport
*
* @brief   Build and send a HID mouse report.
*
* @param   buttons - Mouse button code
*
* @return  none
*/
static void hidKbdMouseSendMouseReport(uint8 buttons, int8 dx, int8 dy, int8 dz)
{
  uint8 buf[HID_MOUSE_IN_RPT_LEN];
  
  buf[0] = buttons;   // Buttons
  buf[1] = dx;         // X
  buf[2] = dy;         // Y
  buf[3] = dz;         // Wheel
  buf[4] = 0;         // AC Pan
  
  HidDev_Report( HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                HID_MOUSE_IN_RPT_LEN, buf );
}

/*********************************************************************
* @fn      hidKbdMouseRcvReport
*
* @brief   Process an incoming HID keyboard report.
*
* @param   len - Length of report.
* @param   pData - Report data.
*
* @return  status
*/
static uint8 hidKbdMouseRcvReport( uint8 len, uint8 *pData )
{
  // verify data length
  if ( len == HID_LED_OUT_RPT_LEN )
  {
    // set keyfob LEDs
    HalLedSet( HAL_LED_1, ((*pData & LED_CAPS_LOCK) == LED_CAPS_LOCK) );
    HalLedSet( HAL_LED_2, ((*pData & LED_NUM_LOCK) == LED_NUM_LOCK) );
    
    return SUCCESS;
  }
  else
  {
    return ATT_ERR_INVALID_VALUE_SIZE;
  }
}

/*********************************************************************
* @fn      hidKbdMouseRptCB
*
* @brief   HID Dev report callback.
*
* @param   id - HID report ID.
* @param   type - HID report type.
* @param   uuid - attribute uuid.
* @param   oper - operation:  read, write, etc.
* @param   len - Length of report.
* @param   pData - Report data.
*
* @return  GATT status code.
*/
static uint8 hidKbdMouseRptCB( uint8 id, uint8 type, uint16 uuid,
                              uint8 oper, uint8 *pLen, uint8 *pData )
{
  uint8 status = SUCCESS;
  
  // write
  if ( oper == HID_DEV_OPER_WRITE )
  {
    if ( uuid == REPORT_UUID )
    {
      // process write to LED output report; ignore others
      if ( type == HID_REPORT_TYPE_OUTPUT )
      {
        status = hidKbdMouseRcvReport( *pLen, pData );
      }
    }
    
    if ( status == SUCCESS )
    {
      status = HidKbM_SetParameter( id, type, uuid, *pLen, pData );
    }
  }
  // read
  else if ( oper == HID_DEV_OPER_READ )
  {
    status = HidKbM_GetParameter( id, type, uuid, pLen, pData );
  }
  // notifications enabled
  else if ( oper == HID_DEV_OPER_ENABLE )
  {
    if ( id == HID_RPT_ID_MOUSE_IN && type == HID_REPORT_TYPE_INPUT )
    {
      hidBootMouseEnabled = TRUE;
    }
  }
  // notifications disabled
  else if ( oper == HID_DEV_OPER_DISABLE )
  {
    if ( id == HID_RPT_ID_MOUSE_IN && type == HID_REPORT_TYPE_INPUT )
    {
      hidBootMouseEnabled = FALSE;
    }
  }
  
  return status;
}

/*********************************************************************
* @fn      hidKbdMouseEvtCB
*
* @brief   HID Dev event callback.
*
* @param   evt - event ID.
*
* @return  HID response code.
*/
static void hidKbdMouseEvtCB( uint8 evt )
{
  // process enter/exit suspend or enter/exit boot mode
  
  return;
}

/* 
Setting up UART
- To use UART, HAL_UART=TRUE, preferrably POWER_SAVING is not enabled
- To use interrupts, HAL_UART_ISR = (1 or 2), HAL_UART_DMA=FALSE
- To use DMA, HAL_UART_ISR = 0, HAL_UART_DMA = (1 or 2)

+ HAL_UART_ISR = 1: Use USART 0
+ HAL_UART_ISR = 2: Use USART 1

Set PERCFG.UxCFG to choose alternative 1 or 2

For keyfob, USART 0 alt. 1 is being used:
- HAL_UART_ISR = 1
For HM-10, USART 1 alt. 2 is being used:
- HAL_UART_ISR = 2

*/

//UART test variable
uint8 *rxBuffer;
uint8 rxBufferIndex = 0;

static void setupUART(void) {
  HalUARTInit();
  
  halUARTCfg_t uartConfig;
  
  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_57600;
  uartConfig.flowControl          = HAL_UART_FLOW_OFF;
  uartConfig.flowControlThreshold = 0;
  uartConfig.rx.maxBufSize        = 8;
  uartConfig.tx.maxBufSize        = 128;
  uartConfig.idleTimeout          = 0;         
  uartConfig.intEnable            = TRUE;
  uartConfig.callBackFunc         = (halUARTCBack_t)uartCallback;
  //uartConfig.callBackFunc         = NULL;
  
  //start UART
  //assumes no issues with starting UART
  (void)HalUARTOpen(HAL_UART_PORT_1, &uartConfig);
  
  rxBuffer = osal_mem_alloc(8); //assumes there is no problem with getting this block of memory
}

static void uartCallback(uint8 port, uint8 event) {    
  uint16 len;
  uint8 buf[8];
  uint8 i;
  
  switch(event) {
  case HAL_UART_RX_FULL:
  case HAL_UART_RX_ABOUT_FULL:
  case HAL_UART_RX_TIMEOUT:
    len = Hal_UART_RxBufLen(HAL_UART_PORT_1);
    HalUARTRead(HAL_UART_PORT_1, buf, len);
    for(i = 0; i < len; i++) {
      if((buf[i] != 0x0D) && (buf[i] != 0x0A)) rxBuffer[rxBufferIndex++] = buf[i];
      else {
        processBuffer();
        break;
      }
    }
    
    break;
  }
}

static void processBuffer(void) {
  //HalUARTWrite(HAL_UART_PORT_1, rxBuffer, rxBufferIndex);

  switch(rxBufferIndex) {
  case 1: //app command
    //printf("One\r\n");
    break;
  case 3: //Keycode commands
    //printf("Three\r\n");
    if(rxBuffer[1] == 'U') {
      //Key released
      KBD_Report_RemoveKey(rxBuffer[2]);
    } else if(rxBuffer[1] == 'D') {
      //Key pressed 
      KBD_Report_AddKey(rxBuffer[2]);
    }
    break;
  case 5: //Mouse command
    //printf("Five\r\n");
    hidKbdMouseSendMouseReport(rxBuffer[1], rxBuffer[2], rxBuffer[3], rxBuffer[4]);
    break;
  case 7:
    //update keyboard report (send to host)
    //printf("Seven\r\n");
    KBD_Report_Update();
    break;
  }
  
  //after processing, reset rxBuffer and its index
  memset(rxBuffer, 0, 8);
  rxBufferIndex = 0;
  
}

static void performPeriodicTask(void) {
  /*
  uint8 *strOut = "Hello World\r";
  HalUARTWrite(HAL_UART_PORT_1, strOut, strlen((const char*)strOut));
  */
}
/*********************************************************************
*********************************************************************/
