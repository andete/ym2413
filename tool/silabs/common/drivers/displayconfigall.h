/***************************************************************************//**
 * @file displayconfigall.h
 * @brief Main configuration file for the DISPLAY driver software stack.
 * @author Energy Micro AS
 * @version 3.20.2
 *******************************************************************************
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
#ifndef __DISPLAYCONFIGALL_H
#define __DISPLAYCONFIGALL_H

/*
 * First, we list the default INCLUDE_XXX #defines which may be excluded later
 * by the kit or application specific configuration files.
 */
#define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE

/* Then include the kit specific display configuration files which also includes
   the application specific configuration file and further selects which modules
   modules to include below. */
#include "displayconfig.h"

#ifdef INCLUDE_DISPLAY_SHARP_LS013B7DH03
#include "displaypalconfig.h"
#include "displayls013b7dh03config.h"
#endif

#ifdef INCLUDE_TEXTDISPLAY_SUPPORT
#include "textdisplayconfig.h"
#include "retargettextdisplayconfig.h"
#endif

#endif /* __DISPLAYCONFIGALL_H */
