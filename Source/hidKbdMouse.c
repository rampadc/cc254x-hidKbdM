/**************************************************************************************************
Filename:       hidKbdMouse.c

Description:    This file contains the HID emulated keyboard sample application
for use with the CC2540 Bluetooth Low Energy Protocol Stack.

Author:         CONG NGUYEN
Last modified:  8/10/2014
**************************************************************************************************/

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

#include "osal_snv.h"

#include "hidKbdMouse.h"
#include <string.h>

#include "KBD_Report.h"
#include "KBD_HUT.h"

/*********************************************************************
* MACROS
*/

//UART testing
#define SBP_PERIODIC_EVT_PERIOD         50
#define KEEP_CONNECTION_ALIVE_50s       50000

// Selected HID LED bitmaps
#define LED_NUM_LOCK                0x01
#define LED_CAPS_LOCK               0x02
#define LED_SCROLL_LOCK             0x03

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

enum Modes {
  command, translate, debug
};

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

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     TRUE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_WAIT_FOR_REQ
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_KEYBOARD_ONLY

//#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_INITIATE
//#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_KEYBOARD_DISPLAY

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           6

#define SNV_ID_DEVICE_NAME              0x80
#define SNV_ID_DEVICE_NAME_LENGTH       0x81
#define SNV_ID_DEVICE_NAME_CRC          0x82

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

static uint8 sleepModeEnabled = 0;

halUARTCfg_t uartConfig;

uint8 *device_name_crc;
uint8 *device_name;
uint8 *device_name_length;

// GAP Profile - Name attribute for SCAN RSP data - Name shown up when scanned
static uint8 default_scanData[] =
{
  0x15,                             // length of this data => (name's size = 20 bytes) + 1
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

/* after calling sleepMode(), the host MCU must wake
HM-xx before sending characters */
static void sleepMode(void);
static void activeMode(void);

static void hidKbdMouse_ProcessOSALMsg( osal_event_hdr_t *pMsg );
//static void hidKbdMouse_HandleKeys( uint8 shift, uint8 keys );
static void hidKbdMouseSendReport( uint8 modifier, uint8 keycode );
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
static void processCommands(void);
static void sendKbdReportsWith(uint8 c);

//uart-to-hid scan code
static const uint8 asciiToKeycodes[128] = {
  hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutBackspace,hutTab,hutEnter,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutReserved,hutEscape,hutReserved,hutReserved,hutReserved,hutReserved,hutSpacebar,hut1,hutApostrophe,hut3,hut4,hut5,hut7,hutApostrophe,hut9,hut0,hut8,hutEqual,hutComma,hutMinus,hutPeriod,hutSlash,hut0,hut1,hut2,hut3,hut4,hut5,hut6,hut7,hut8,hut9,hutSemicolon,hutSemicolon,hutComma,hutEqual,hutPeriod,hutSlash,hut2,hutA,hutB,hutC,hutD,hutE,hutF,hutG,hutH,hutI,hutJ,hutK,hutL,hutM,hutN,hutO,hutP,hutQ,hutR,hutS,hutT,hutU,hutV,hutW,hutX,hutY,hutZ,hutLeftBracket,hutBackslash,hutRightBracket,hut6,hutMinus,hutTilde,hutA,hutB,hutC,hutD,hutE,hutF,hutG,hutH,hutI,hutJ,hutK,hutL,hutM,hutN,hutO,hutP,hutQ,hutR,hutS,hutT,hutU,hutV,hutW,hutX,hutY,hutZ,hutLeftBracket,hutBackslash,hutRightBracket,hutTilde,hutDeleteForward
};

//Pololu's CRC functions with minimal changes
uint8 CRCPoly = 0x89;  // the value of our CRC-7 polynomial
uint8 CRCTable[256];

void GenerateCRCTable()
{
  int i, j;

  // generate a table value for all 256 possible byte values
  for (i = 0; i < 256; i++)
  {
    CRCTable[i] = (i & 0x80) ? i ^ CRCPoly : i;
    for (j = 1; j < 8; j++)
    {
      CRCTable[i] <<= 1;
      if (CRCTable[i] & 0x80)
        CRCTable[i] ^= CRCPoly;
    }
  }
}


// adds a message byte to the current CRC-7 to get a the new CRC-7
uint8 CRCAdd(uint8 CRC, uint8 message_byte)
{
  return CRCTable[(CRC << 1) ^ message_byte];
}


// returns the CRC-7 for a message of "length" bytes
uint8 getCRC(uint8 message[], uint8 length)
{
  uint8 i;
  uint8 CRC = 0;

  for (i = 0; i < length; i++)
    CRC = CRCAdd(CRC, message[i]);

  return CRC;
}

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
  setupUART();
  GenerateCRCTable();
  //printf("%x\n",default_name_crc);
  device_name = osal_mem_alloc(20);
  device_name_length = osal_mem_alloc(1);
  device_name_crc = osal_mem_alloc(1);

  hidKbdMouseTaskId = task_id;

  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );

  // Setup the GAP Peripheral Role Profile
  {
    uint8 initial_advertising_enable = TRUE; //previously FALSE

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

    //name change - 21/12/2014
    osal_snv_read(SNV_ID_DEVICE_NAME_CRC, 1, device_name_crc);
    osal_snv_read(SNV_ID_DEVICE_NAME_LENGTH, 1, device_name_length);
    osal_snv_read(SNV_ID_DEVICE_NAME, 20, device_name);

    if(*device_name_crc != getCRC(device_name, *device_name_length)) {
      printf("Using default scan response name\r\n");
      GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( default_scanData ), default_scanData );
    } else {
      //make changes directly to the default_scanData. Since this variable is set at start-up, it should not matter
      uint8 len = *device_name_length;
      //uint8 default_name_length = default_scanData[0];
      default_scanData[0] = len + 1;
      uint8 i;
      for(i = 0; i < len; i++) {
        default_scanData[i+2] = device_name[i];
      }
      GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( default_scanData ), default_scanData );
    }

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  if(*device_name_crc != getCRC(device_name, *device_name_length)) {
    //    printf("Using default device name");
    GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *) attDeviceName );
  } else {
    //    printf("Using stored device name");
    //    printf("%s\r\n", device_name);
    GGS_SetParameter( GGS_DEVICE_NAME_ATT, *device_name_length + 1, (void *) device_name );
  }
  //Allow device to change name
  uint8 devNamePermission = GATT_PERMIT_READ|GATT_PERMIT_WRITE;
  GGS_SetParameter( GGS_W_PERMIT_DEVICE_NAME_ATT, sizeof ( uint8 ), &devNamePermission );

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
  P1 = 0xFF;   // All pins on port 1 to high
  P2 = 0;   // All pins on port 2 to low

#endif // #if defined( CC2540_MINIDK )
  
  // Turn on an LED
  //init keyboard report manager
  KBD_Report_Init();

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
    osal_start_timerEx( hidKbdMouseTaskId, SBP_PERIODIC_EVT, KEEP_CONNECTION_ALIVE_50s );

    return ( events ^ START_DEVICE_EVT );
  }

  //Added to test UART
  if ( events & SBP_PERIODIC_EVT )
  {
    // Restart timer
    if ( KEEP_CONNECTION_ALIVE_50s )
    {
      osal_start_timerEx( hidKbdMouseTaskId, SBP_PERIODIC_EVT, KEEP_CONNECTION_ALIVE_50s );
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
    //  case KEY_CHANGE:
    //    hidKbdMouse_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
    //    break;

  default:
    break;
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
static void hidKbdMouseSendReport( uint8 modifier, uint8 keycode )
{
  uint8 buf[HID_KEYBOARD_IN_RPT_LEN];

  buf[0] = modifier;  // Modifier keys
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
  //attempt to extract report bits from host output
  if(len == HID_LED_OUT_RPT_LEN) {
    if((*pData & LED_CAPS_LOCK) == LED_CAPS_LOCK) printf("CL,1");
    else printf("CL,0");

    if((*pData & LED_NUM_LOCK) == LED_NUM_LOCK) printf("NL,1");
    else printf("NL, 0");

    if((*pData & LED_SCROLL_LOCK) == LED_SCROLL_LOCK) printf("SL,1");
    else printf("SL,0");

    return SUCCESS;
  } else {
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
For HM-11, RX on P0.2, TX on P0.3 => USART 0 alt. 1 is being used:
- HAL_UART_ISR = 1
*/

//UART test variable
uint8 *rxBuffer;
uint8 rxBufferIndex = 0;

uint8 *modeSelStr;
uint8 strIndex = 0;

uint8 mode = translate;

static void setupUART(void) {
  HalUARTInit();

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_57600;
  uartConfig.flowControl          = HAL_UART_FLOW_OFF;
  uartConfig.flowControlThreshold = 0;
  uartConfig.rx.maxBufSize        = 20;
  uartConfig.tx.maxBufSize        = 128;
  uartConfig.idleTimeout          = 0;
  uartConfig.intEnable            = TRUE;
  uartConfig.callBackFunc         = (halUARTCBack_t)uartCallback;
  //uartConfig.callBackFunc         = NULL;

  //start UART
  //assumes no issues with starting UART
#if (HAL_UART_ISR == 1)
  (void)HalUARTOpen(HAL_UART_PORT_0, &uartConfig);
#else
  (void)HalUARTOpen(HAL_UART_PORT_1, &uartConfig);
#endif
  
  //printf("Started UART\r\n");

  //assumes there is no problem with getting these blocks of bytes
  rxBuffer = osal_mem_alloc(20); //expanded to handle name changes
  modeSelStr = osal_mem_alloc(3);
}

static void uartCallback(uint8 port, uint8 event) {
  uint16 len;
  uint8 buf[8];
  uint8 i;

  switch(event) {
  case HAL_UART_RX_FULL:
  case HAL_UART_RX_ABOUT_FULL:
  case HAL_UART_RX_TIMEOUT:
#if (HAL_UART_ISR == 1)
    len = Hal_UART_RxBufLen(HAL_UART_PORT_0);
    HalUARTRead(HAL_UART_PORT_0, buf, len);
#else
    len = Hal_UART_RxBufLen(HAL_UART_PORT_1);
    HalUARTRead(HAL_UART_PORT_1, buf, len);
#endif

    /*
    Proposing states:
    - If 3 consequence @'s, ie. @@@ is sent, put device into command mode (0)
    - If 3 consequent $'s, ie. $$$ is sent, put device into translation mode (1)

    - In command mode, use U<value>, D<value>, M<value><value><value><value>, S<value>,<value>
    all followed by a line return
    - In translation mode, if the buffer is within ASCII printable, create
    reports with corresponding USB HID keycode and send to host (translate). Mouse data is sent with
    0x03 <state> <x> <y> <z>
    */

    for(i = 0; i < len; i++) {

      //Detects if a mode is being selected
      if((buf[i] == '@') || (buf[i] == '$')) { //not to waste time
        //printf("strIndex: %i\r\n",strIndex);
        modeSelStr[strIndex++] = buf[i];
        if(strIndex == 3) {
          //printf("Testing for selection\r\n");
          if((modeSelStr[0] == '@') && (modeSelStr[1] == '@') && (modeSelStr[2] == '@')) {
            mode = 0;
          } else if((modeSelStr[0] == '$') && (modeSelStr[1] == '$') && (modeSelStr[2] == '$')) {
            mode = 1;
          }
          strIndex = 0;
          memset(modeSelStr, 0, 3);
          break; //stops filling buffer if a new mode is selected
        }
      } else {
        memset(modeSelStr, 0, 3);
        strIndex = 0;
      }

      if(mode == command) {
        //command mode is selected
        if((buf[i] != 0x0D) && (buf[i] != 0x0A)) {
          rxBuffer[rxBufferIndex++] = buf[i];
        } else {
          processCommands();
          if(sleepModeEnabled) {
            sleepMode();
          }
          break;
        }
      } else if (mode == translate) {
        sendKbdReportsWith(buf[i]);
      } else if (mode == debug) {
        
      }
    }

    break; //break for case(HAL_UART_RX_TIMEOUT)
  }
}

/*
Translate c to keyboard reports
*/
static void sendKbdReportsWith(uint8 c) {
  uint8 modifier = 0;
  uint8 keycode = asciiToKeycodes[c];

  //convert ascii characters to keycodes
  if(((c >= 33) && (c <=38))||(c==40)||(c==41)||(c==42)||(c==43)||(c==58)||(c==60)||(c==94)||(c==95)||(c==123)||(c==124)||(c==125)||(c==126)||((c >= 62) && (c<=90))) {
    modifier |= 0x02; //Left Shift
  }

  //emulate a key being pressed then released
  hidKbdMouseSendReport(modifier,keycode);
  hidKbdMouseSendReport(0,0);
}

//Buffer being processed stored in rxBuffer, does not include CRLF
/*
Command sets, chosen options need to be stored in non-volatile memory
- SC,<value>  + set connection mode of device
+ <value> is a single digit number
+ 1 Do not auto-connect to last paired, 2 auto-connect to last paired
- SN,<value>  + set device name
+ <value> device's new name
- S,R Reset the device
- S,D Set device to be discoverable
- S,DC  Disconnect device from host
*/
static void processCommands(void) {
  //buf: Testing variables
  uint8 *buf = rxBuffer;
  buf[rxBufferIndex] = 0;

  if(rxBuffer[0] == 'K') { //keyboard commands
    if(rxBufferIndex == 3) {
      if (rxBuffer[1] == 'M') {
        // Modifier
        
      } else if (rxBuffer[1] == 'S') {
         KBD_Report_AddKey(rxBuffer[2]);
         KBD_Report_Update();
         KBD_Report_RemoveKey(rxBuffer[2]);
         KBD_Report_Update();
      } else if(rxBuffer[1] == 'U') {
        //Key released
        printf("key released\r\n");
        KBD_Report_RemoveKey(rxBuffer[2]);
      } else if(rxBuffer[1] == 'D') {
        //Key pressed
        printf("key pressed\r\n");
        KBD_Report_AddKey(rxBuffer[2]);
      }
    } else { //command as KUPDATE
      printf("Sending report\r\n");
      KBD_Report_Update();
    }
  } else if(rxBuffer[0] == 'M') { //mouse commands
    //printf("Mouse commands\r\n");
    hidKbdMouseSendMouseReport(rxBuffer[1], rxBuffer[2], rxBuffer[3], rxBuffer[4]);
  } else if(rxBuffer[0] == 'G') { // getting commands
    printf("Get commands\r\n");
    if (rxBuffer[1] == ',') {
      if (rxBuffer[2] == 'M') {
        printf(mode == command ? "CMD\r\n" : "TRANS\r\n");
      }
    }
  } else if(rxBuffer[0] == 'S') { //setting commands
    printf("Setting commands\r\n");
    if((rxBuffer[1] == 'C') && (rxBuffer[2] == ',')) {
      //TO-DO: SET CONNECTION MODE
      printf("Connection modes\r\n");
    } else if((rxBuffer[1] == 'N') && (rxBuffer[2] == ',')) {
      uint8 i;
      uint8 deviceNewName[20];
      uint8 deviceNewNameLength;
      uint8 deviceNewNameCRC;

      deviceNewNameLength = rxBufferIndex-3;
      if(deviceNewNameLength > 20) {
        printf("Name exceeds permitted length\r\n");
      } else {
        for(i = 3; i < rxBufferIndex; i++) {
          deviceNewName[i-3] = rxBuffer[i];
        }
        deviceNewName[deviceNewNameLength] = '\0';
        deviceNewNameCRC = getCRC(deviceNewName, deviceNewNameLength);

        osal_snv_write(SNV_ID_DEVICE_NAME, 20, deviceNewName);
        osal_snv_write(SNV_ID_DEVICE_NAME_LENGTH, 1, &deviceNewNameLength);
        osal_snv_write(SNV_ID_DEVICE_NAME_CRC, 1, &deviceNewNameCRC);
        printf("Name is being set, reset to set new name\r\n");
      }
    } else if((rxBuffer[1] == ',') && (rxBuffer[2] == 'R')) {
      //reset the device
      HAL_SYSTEM_RESET();
    } else if((rxBuffer[1] == ',') && (rxBuffer[2] == 'D')) {
      if(rxBuffer[3] == 'C') {
        //disconnect the device from host
        printf("Disconnecting from host...\r\n");
        GAPRole_TerminateConnection();
      } else {
        //set device to be discoverable
        printf("Set deveice to be discoverable\r\n");
      }
    } else if((rxBuffer[1] == ',') && (rxBuffer[2] == 'S')) {
      //enable sleep mode
      printf("SLEEP\r\n");
      sleepModeEnabled = 1;
      sleepMode();
    } else if((rxBuffer[1] == ',') && (rxBuffer[2] == 'A')) {
      //disable sleep mode
      printf("ACTIVE\r\n");
      sleepModeEnabled = 0;
      activeMode();
    }
  }

  //after processing, reset rxBuffer and its index
  memset(rxBuffer, 0, 8);
  rxBufferIndex = 0;
}

static void performPeriodicTask(void) {
  //send a blank keyboard report to keep connection connected
  //  hidKbdMouseSendReport(0,hut5);
  hidKbdMouseSendReport(0,0);
  //printf("Keep Alive\r\n");
}

static void sleepMode(void) {
  //Configure RX pin as input with active high interrupt
  // - HM-10/HM-11: RX = P1.7
#if (HAL_UART_ISR == 1)
  P0SEL &= ~(1 << 2); //deselect as peripheral pin, just to be sure
  P0DIR &= ~(1 << 2); //input
  P0INP &= ~(1 << 2); //enable pull-up or pull-down
  P2INP &= ~(1 << 5); //select pull-up for all port 0 pins - active low
  PICTL |= (1 << 0);  //Falling edge on input gives interrupt (P0ICON)
  P0IFG &= ~(1 << 2); //Clear existing interrupt on P0.2
  P0IEN |= ~(1 << 2); //Enable interrupt on P0.2
#else
  //HM-10
  P1SEL &= ~(1 << 7); //deselect as peripheral pin, just to be sure
  P1DIR &= ~(1 << 7); //input
  P1INP &= ~(1 << 7); //enable pull-up or pull-down
  P2INP &= ~(1 << 6); //select pull-up for all port 1 pins - active low
  PICTL |= (1 << 2);  //Falling edge on input gives interrupt (P1ICONH)
  P0IFG &= ~(1 << 7); //Clear existing interrupt on P0.2
  P0IEN |= ~(1 << 7); //Enable interrupt on P0.2
#endif
  osal_pwrmgr_device( PWRMGR_BATTERY );
}

static void activeMode(void) {
  // Disable pin interrupts and re-enable UART
  // - HM-10: RX = P1.7
#if (HAL_UART_ISR == 1)
  P0SEL |= (1 << 2); //select as peripheral pin
  (void)HalUARTOpen(HAL_UART_PORT_0, &uartConfig);
#else
  //HM-10
  P1SEL |= (1 << 7); //select as peripheral pin
  (void)HalUARTOpen(HAL_UART_PORT_1, &uartConfig);
#endif
  osal_pwrmgr_device( PWRMGR_ALWAYS_ON );
}
/*********************************************************************
ISR
*********************************************************************/
#pragma vector=P0INT_VECTOR
__interrupt void P0_ISR(void) {
  //Disable pin interrupts and re-enable UART

  HAL_ENTER_ISR();
  //clear interrupt
  P0IFG = 0;
  P0IF = 0;

  uint8 pV = P0_2;
  if(pV == 0) {
    activeMode();
  }
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

#pragma vector=P1INT_VECTOR
__interrupt void P1_ISR(void) {
  //HM-10 - Disable pin interrupts and re-enable UART

  HAL_ENTER_ISR();
  P1IFG = 0;
  P1IF = 0;

  uint8 pV = P1_7;
  if(pV) {
    //active low
    activeMode();
  }
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}