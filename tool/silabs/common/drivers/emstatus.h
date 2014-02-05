/**************************************************************************//**
 * @file emstatus.h
 * @brief EMSTATUS definitions.
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
#ifndef _EMSTATUS_H_
#define _EMSTATUS_H_

#include <stdint.h>
#include "em_types.h"          /* typedef uint32_t EMSTATUS; */

/** Common EMSTATUS codes: */
#define EMSTATUS_OK                   (0)            /**< Operation successful. */

/** EMSTATUS base codes for display modules. */
#define DISPLAY_EMSTATUS_BASE         (0x8011UL<<16) /**< EMSTATUS base code of DISPLAY driver interface. */
#define TEXTDISPLAY_EMSTATUS_BASE     (0x8012UL<<16) /**< EMSTATUS base code of TEXTDISPLAY module. */
#define PAL_EMSTATUS_BASE             (0x8013UL<<16) /**< EMSTATUS base code of PAL interface. */

#endif  /*  _EMSTATUS_H_  */
