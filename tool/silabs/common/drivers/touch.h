/***************************************************************************//**
 * @file
 * @brief Touch panel driver prototypes and definitions
 * @author Energy Micro AS
 * @version 3.20.2
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
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

#ifndef __TOUCH_H
#define __TOUCH_H

#include "em_device.h"
#include "em_adc.h"
#include "calibrate.h"

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Touch
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define TOUCH_IDLE         0
#define TOUCH_BUSY_PRESSED 1
#define TOUCH_BUSY_CHECK   2
#define TOUCH_BUSY_SCAN    3
/** @endcond */

/** Touch panel position structure */
typedef struct
{ uint16_t x;           /**< x position */
  uint16_t y;           /**< y position */
  uint16_t adcx;        /**< ADC X readings */
  uint16_t adcy;        /**< ADC Y readings */
  uint8_t  pen;         /**< 0=not pressed, 1=touched */
} TOUCH_Pos_TypeDef;

/** Touch configuration structure */
typedef struct
{ uint32_t               frequency;     /**< ADC frequency, min 32kHz, max 13MHz, typical 500kHz */
  uint8_t                ignore;        /**< Ignore move if it has not at least defined pixels */
  ADC_OvsRateSel_TypeDef oversampling;  /**< ADC oversampling used during touch reading */
} TOUCH_Config_TypeDef;

/** Initializes Touch default values */
#define TOUCH_INIT_DEFAULT                                             \
  { 500000,                /* 500kHz sampling frequency */             \
    1,                     /* Ignore moves not greater than 1 pixel */ \
    adcOvsRateSel128,      /* Oversampling 128 times */                \
  }

/** touch panel upcall type definition */
typedef void (TOUCH_Upcall_TypeDef)(TOUCH_Pos_TypeDef *);

void TOUCH_Init(TOUCH_Config_TypeDef *config);
void TOUCH_RegisterUpcall(TOUCH_Upcall_TypeDef *new_upcall);
TOUCH_Pos_TypeDef *TOUCH_GetPos(void);
int TOUCH_CalibrationTable(POINT * displayPtr, POINT * screenPtr);
int TOUCH_IsBusy(void);

#ifdef __cplusplus
}
#endif

/** @} (end group Touch) */
/** @} (end group Drivers) */

#endif
