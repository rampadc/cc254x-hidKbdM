/**************************************************************************************************
  Filename:       OSAL_hidKbdMouse.c
  Description:    This file contains function that allows user setup tasks
*******************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"

/* HAL */
#include "hal_drivers.h"

/* LL */
#include "ll.h"

/* HCI */
#include "hci_tl.h"

#if defined ( OSAL_CBTIMER_NUM_TASKS )
  #include "osal_cbTimer.h"
#endif

/* L2CAP */
#include "l2cap.h"

/* gap */
#include "gap.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

/* GATT */
#include "gatt.h"

#include "gattservapp.h"

/* Profiles */
#include "peripheral.h"
#include "hiddev.h"

/* Application */
#include "hidKbdMouse.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// The order in this table must be identical to the task initialization calls below in osalInitTask.
const pTaskEventHandlerFn tasksArr[] =
{
  LL_ProcessEvent,
  Hal_ProcessEvent,
  HCI_ProcessEvent,
#if defined ( OSAL_CBTIMER_NUM_TASKS )
  OSAL_CBTIMER_PROCESS_EVENT( osal_CbTimerProcessEvent ),
#endif
  L2CAP_ProcessEvent,
  GAP_ProcessEvent,
  GATT_ProcessEvent,
  SM_ProcessEvent,
  GAPRole_ProcessEvent,
  GAPBondMgr_ProcessEvent,
  GATTServApp_ProcessEvent,
  HidDev_ProcessEvent,
  HidKbdMouse_ProcessEvent
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  /* LL Task */
  LL_Init( taskID++ );

  /* Hal Task */
  Hal_Init( taskID++ );

  /* HCI Task */
  HCI_Init( taskID++ );

#if defined ( OSAL_CBTIMER_NUM_TASKS )
  /* Callback Timer Tasks */
  osal_CbTimerInit( taskID );
  taskID += OSAL_CBTIMER_NUM_TASKS;
#endif

  /* L2CAP Task */
  L2CAP_Init( taskID++ );

  /* GAP Task */
  GAP_Init( taskID++ );

  /* GATT Task */
  GATT_Init( taskID++ );

  /* SM Task */
  SM_Init( taskID++ );

  /* Profiles */
  GAPRole_Init( taskID++ );
  GAPBondMgr_Init( taskID++ );

  GATTServApp_Init( taskID++ );

  HidDev_Init( taskID++ );

  /* Application */
  HidKbdMouse_Init( taskID );
}

/*********************************************************************
*********************************************************************/
