/**************************************************************************//**
 * @file textdisplay.h
 * @brief Text display interface
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2013 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/

#ifndef _TEXTDISPLAY_H_
#define _TEXTDISPLAY_H_

#include <stdbool.h>
#include "emstatus.h"

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Textdisplay
 * @brief Line based text output terminal interface on top of the DISPLAY
 *        device driver stack. See @ref textdisplay_doc for more information.
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ********************************  DEFINES  ************************************
 ******************************************************************************/

/** EMSTATUS codes of the textdisplay module. */
#define TEXTDISPLAY_EMSTATUS_OK                                              (0) /**< Operation successful. */
#define TEXTDISPLAY_EMSTATUS_INVALID_PARAM       (TEXTDISPLAY_EMSTATUS_BASE | 1) /**< Invalid parameter. */
#define TEXTDISPLAY_EMSTATUS_OUT_OF_RANGE        (TEXTDISPLAY_EMSTATUS_BASE | 2) /**< Parameters out of range. */
#define TEXTDISPLAY_EMSTATUS_ALREADY_INITIALIZED (TEXTDISPLAY_EMSTATUS_BASE | 3) /**< Device is already initialized. */
#define TEXTDISPLAY_EMSTATUS_NOT_SUPPORTED       (TEXTDISPLAY_EMSTATUS_BASE | 4) /**< Feature/option not supported. */
#define TEXTDISPLAY_EMSTATUS_NOT_ENOUGH_MEMORY   (TEXTDISPLAY_EMSTATUS_BASE | 5) /**< Not enough memory. */
#define TEXTDISPLAY_EMSTATUS_NOT_INITIALIZED     (TEXTDISPLAY_EMSTATUS_BASE | 6) /**< Device is not initialized. */


/** VT52/VT100 Escape Sequence codes supported by TEXTDISPLAY: */
#ifdef INCLUDE_VIDEO_TERMINAL_ESCAPE_SEQUENCE_SUPPORT
#define TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100       "\033[H" /**< Move cursor to upper left corner. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT52         "\033H" /**< Move cursor to upper left corner. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE       "\033A" /**< Move cursor up one line. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_DOWN_ONE_LINE     "\033B" /**< Move cursor down one line. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR    "\033C" /**< Move cursor right one column. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR     "\033D" /**< Move cursor left one column. */
#endif


/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** Handle for text display device. */
typedef void*  TEXTDISPLAY_Handle_t;

/** Initialization data structure for text display devices. */
typedef struct TEXTDISPLAY_Config_t
{
  int   displayDeviceNo;    /**< Display device number to initialize test
                                 display on. */
  bool  scrollEnable;       /**< Enable/disable scrolling mode on the text
                                 display. Scrolling mode will consume more
                                 memory because the lines will have to be
                                 stored in memory. */
  
  bool  lfToCrLf;           /**< Enable or disable LF to CR+LF conversion. */
} TEXTDISPLAY_Config_t;


/*******************************************************************************
 **************************    FUNCTION PROTOTYPES    **************************
 ******************************************************************************/

EMSTATUS TEXTDISPLAY_New         (TEXTDISPLAY_Config_t  *config,
                                  TEXTDISPLAY_Handle_t  *handle);
EMSTATUS TEXTDISPLAY_Delete      (TEXTDISPLAY_Handle_t  handle);
EMSTATUS TEXTDISPLAY_WriteChar   (TEXTDISPLAY_Handle_t  handle,
                                  char                  c);
EMSTATUS TEXTDISPLAY_WriteString (TEXTDISPLAY_Handle_t  handle,
                                  char*                 str);
EMSTATUS TEXTDISPLAY_LfToCrLf    (TEXTDISPLAY_Handle_t  handle,
                                  bool                  on);

#ifdef __cplusplus
}
#endif

/** @} (end group TEXTDISPLAY) */
/** @} (end group Drivers) */

#endif /* _TEXTDISPLAY_H_ */
