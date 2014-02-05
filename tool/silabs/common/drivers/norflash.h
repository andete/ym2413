/***************************************************************************//**
 * @file  norflash.h
 * @brief Definitions for the NORFLASH driver for Spansion S29GL128P90FFIR13.
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
#ifndef __NORFLASH_H
#define __NORFLASH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup NorFlash
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief NORFLASH status enumerator. */
typedef enum
{
  NORFLASH_STATUS_OK           = 0,             /**< No errors detected.                      */
  NORFLASH_NONUNIFORM_GEOMETRY = -1,            /**< The flash has non-uniform sector scheme. */
  NORFLASH_NOT_CFI_DEVICE      = -2,            /**< The flash is not CFI compliant.          */
  NORFLASH_WRITE_FAILURE       = -3,            /**< Flash write/erase failure.               */
  NORFLASH_WRITE_TIMEOUT       = -4,            /**< Flash write/erase timeout.               */
  NORFLASH_INVALID_ADDRESS     = -5,            /**< Invalid flash address.                   */
  NORFLASH_MISALIGNED_ADDRESS  = -6,            /**< Misaligned flash word address.           */
} NORFLASH_Status_TypeDef;

/** @brief NORFLASH device information struct. */
typedef struct
{
  uint32_t baseAddress;               /**< The device base address in cpu memory map. */
  uint32_t deviceId;                  /**< The device ID.                             */
  uint32_t deviceSize;                /**< Total device size in bytes.                */
  uint32_t sectorCount;               /**< Number of sectors in the device.           */
  uint32_t sectorSize;                /**< Sector size in bytes.                      */
  uint16_t manufacturerId;            /**< The device manufacturer ID.                */
} NORFLASH_Info_TypeDef;

bool                    NORFLASH_AddressValid(uint32_t addr);
NORFLASH_Info_TypeDef*  NORFLASH_DeviceInfo(void);
int                     NORFLASH_EraseDevice(void);
int                     NORFLASH_EraseSector(uint32_t addr);
int                     NORFLASH_Init(void);
int                     NORFLASH_ProgramByte(uint32_t addr, uint8_t data);
int                     NORFLASH_ProgramWord16(uint32_t addr, uint16_t data);
int                     NORFLASH_ProgramWord32(uint32_t addr, uint32_t data);
int                     NORFLASH_Program(uint32_t addr, uint8_t *data, uint32_t count);

#ifdef __cplusplus
}
#endif

/** @} (end group NorFlash) */
/** @} (end group Drivers) */

#endif /* __NORFLASH_H */
