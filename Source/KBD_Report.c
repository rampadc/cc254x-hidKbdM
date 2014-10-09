#include "KBD_Report.h"
#include "hiddev.h"

/* Local functions */
static bool findEmptySlot(uint8 *slot);
static bool KBD_Report_isFull(void);
static bool KBD_Report_isEmpty(void);
static void KBD_Report_sendReport(void);

/* Local variables */
static ReportBuffer_t Report_buff;

/* Public functions */

/*******************************************************************************
@brief      Initialize the HID keyboard buffer
@param      none    
@return     none
*******************************************************************************/
void KBD_Report_Init(void) {
  uint8 i;
  Report_buff.update = FALSE;
  Report_buff.emptySlots.byte = ALL_SLOTS;
  Report_buff.size = REPORT_BUFFER_SIZE;
  for(i = 0; i < REPORT_BUFFER_SIZE; i++) {
    Report_buff.Buff[i] = 0;
  }
}

/*******************************************************************************
@brief      Add key to report buffer
@param      HID keycode of the key    
@return     TRUE if added, FALSE if buffer is full
*******************************************************************************/
bool KBD_Report_AddKey(uint8 keycode) {
  uint8 s;
  
  //Run this in case the key is already in buffer
  KBD_Report_RemoveKey(keycode);
  
  //Check if this is a modifier
  if((keycode & 0xF0) == 0xE0) {
    Report_buff.Buff[MODIFIER_OFFSET] |= (1<<(keycode & 0x0F));
    Report_buff.update = TRUE;
    return TRUE;
  } else {
    //This is an element array
    if(findEmptySlot(&s) == TRUE) {
      //Write key to buffer
      Report_buff.Buff[s] = keycode;
      //Clear empty slot bit
      Report_buff.emptySlots.byte &= ~(1<<s);
      Report_buff.update = TRUE;
      return TRUE;
    } else {
      return FALSE;
    }
  }
}

/*******************************************************************************
@brief      Remove key from buffer
@param      HID keycode of the key    
@return     TRUE if removed, FALSE if not found
*******************************************************************************/
bool KBD_Report_RemoveKey(uint8 keycode) {
  uint8 i;
  
  //Check if this is a modifier
  if((keycode & 0xF0) == 0xE0) {
    Report_buff.Buff[MODIFIER_OFFSET] &= ~(1<<(keycode & 0x0F));
    Report_buff.update = TRUE;
  }
  for(i = DATA_OFFSET; i < REPORT_BUFFER_SIZE; i++) {
    //Look for key in buffer
    if((((Report_buff.emptySlots.byte) & (1<<i)) == 0) && (Report_buff.Buff[i]) == keycode) {
      Report_buff.emptySlots.byte |= (1<<i);
      Report_buff.Buff[i] = 0;
      Report_buff.update = TRUE;
      return TRUE;
    }
  }
  
  //Key can't be found
  return FALSE;
}

/*******************************************************************************
@brief      Send updated report through Bluetooth HID
@param      none
@return     none
*******************************************************************************/
void KBD_Report_Update(void) {
  if(Report_buff.update == TRUE) {
    KBD_Report_sendReport();
    Report_buff.update = FALSE;
  }
}

/* Local functions definitions */

/*******************************************************************************
@brief      Find empty slot
@param      none
@return     none
*******************************************************************************/
static bool findEmptySlot(uint8 *slot) {
  uint8 i;
  if(KBD_Report_isFull() == FALSE) {
    for(i = DATA_OFFSET; i < REPORT_BUFFER_SIZE; i++) {
      if(((Report_buff.emptySlots.byte) & (1<<i)) != 0x00) {
        *slot = i;
        return TRUE;
      }
    }
  }
  //Nothing found
  return FALSE;
}

/*******************************************************************************
@brief      Checks if the report buffer is full
@param      none
@return     TRUE if full, FALSE if not
*******************************************************************************/
static bool KBD_Report_isFull(void) {
  if(Report_buff.emptySlots.byte == 0x00) return TRUE;
  else return FALSE;
}

/*******************************************************************************
@brief      Checks if report buffer is empty
@param      none
@return     TRUE if empty, FALSE if not
*******************************************************************************/
static bool KBD_Report_isEmpty(void) {
  if(Report_buff.emptySlots.byte == ALL_SLOTS) return TRUE;
  else return FALSE;
}

/*******************************************************************************
@brief      Sends the report via Bluetooth to the host
@param      none
@return     none
*******************************************************************************/
static void KBD_Report_sendReport(void)
{
  HidDev_Report(HID_RPT_ID_KEY_IN, 
                HID_REPORT_TYPE_INPUT,
                HID_KEYBOARD_IN_RPT_LEN, 
                (uint8*)&Report_buff.Buff[0]);
}