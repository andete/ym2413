/***************************************************************************//**
 * @file  msdd.h
 * @brief Mass Storage class Device (MSD) driver.
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
#ifndef __MSDD_H
#define __MSDD_H

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Msd
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_BUFSIZ    4096      /**< Intermediate media storage buffer size */

/**************************************************************************//**
 * @brief Status info for one BOT CBW -> Data I/O -> CSW cycle.
 *****************************************************************************/
typedef struct
{
  bool                                                       valid;     /**< True if the CBW is valid.    */
  uint8_t                                                    direction; /**< Set if BOT direction is IN.  */
  uint8_t                                                    *pData;    /**< Media data pointer.          */
  uint32_t                                                   lba;       /**< SCSI Read/Write lba address. */
  uint32_t                                                   xferLen;   /**< BOT transfer length.         */
  uint32_t                                                   maxBurst;  /**< Max length of one transfer.  */
  enum              { XFER_MEMORYMAPPED = 0, XFER_INDIRECT } xferType;
  /**< MSD media access type.       */
} MSDD_CmdStatus_TypeDef;

/*** MSDD Device Driver Function prototypes ***/

bool MSDD_Handler(void);
void MSDD_Init(int activityLedPort, uint32_t activityLedPin);

#ifdef __cplusplus
}
#endif

/** @} (end group Msd) */
/** @} (end group Drivers) */

#endif /* __MSDD_H */
