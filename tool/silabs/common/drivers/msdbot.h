/***************************************************************************//**
 * @file  msdbot.h
 * @brief Definitions for the Bulk Only Transport protocol of
 *        USB Mass Storage devices.
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
#ifndef __MSDBOT_H
#define __MSDBOT_H

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

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define CBW_LEN          31
#define CSW_LEN          13

#define BOT_DIR_IN       0x80                 /* bmCBWFlags value for bulk IN cmds.  */
#define BOT_DIR_OUT      0x00                 /* bmCBWFlags value for bulk OUT cmds. */

#define CSW_SIGNATURE    0x53425355           /* Ascii USBS */
#define CBW_SIGNATURE    0x43425355           /* Ascii USBC */

/** @endcond */

/**************************************************************************//**
 * @brief Bulk Only Transport (BOT) Command Block Wrapper (CBW) typedef.
 *****************************************************************************/
EFM32_PACK_START(1)
typedef struct
{
  uint32_t dCBWSignature;           /**< The CBW signature (ascii USBC). */
  uint32_t dCBWTag;                 /**< The CBW tag.                    */
  uint32_t dCBWDataTransferLength;  /**< BOT transfer length.            */
  union
  {
    struct
    {
      uint8_t Reserved1 : 6;        /**< Reserved, expect 0.             */
      uint8_t Obsolete  : 1;        /**< Obsolete, expect 0.             */
      uint8_t Direction : 1;        /**< BOT transfer directon, 1=BOT IN */
    };
    uint8_t bmCBWFlags;             /**< CBW Flags.                      */
  };
  union
  {
    struct
    {
      uint8_t Lun       : 4;        /**< Device Logical Unit Number.     */
      uint8_t Reserved2 : 4;        /**< Reserved, expect 0.             */
    };
    uint8_t bCBWLUN;
  };
  union
  {
    struct
    {
      uint8_t CBLength  : 5;        /**< SCSI CDB length.                */
      uint8_t Reserved3 : 3;        /**< Reserved, expect 0.             */
    };
    uint8_t bCBWCBLength;           /**< SCSI CDB length.                */
  };
  uint8_t CBWCB[ 16 ];              /**< SCSI CDB.                       */
} __attribute__ ((packed)) MSDBOT_CBW_TypeDef;
EFM32_PACK_END()

/**************************************************************************//**
 * @brief Bulk Only Transport (BOT) Command Status Wrapper (CSW) typedef.
 *****************************************************************************/
EFM32_PACK_START(1)
typedef struct
{
  uint32_t dCSWSignature;           /**< The CSW signature (ascii USBS). */
  uint32_t dCSWTag;                 /**< The CSW tag.                    */
  uint32_t dCSWDataResidue;         /**< BOT transfer residue count.     */
  uint8_t  bCSWStatus;              /**< BOT command execution result.   */
} __attribute__ ((packed)) MSDBOT_CSW_TypeDef;
EFM32_PACK_END()

/** @brief MSDBOT status enumerator. */
typedef enum
{
  MSDBOT_STATUS_OK  = 0,            /**< MSDBOT status, no error.        */
  MSDBOT_INIT_ERROR = -1,           /**< MSDBOT initialization failure.  */
  MSDBOT_CMD_FAILED = -2,           /**< MSDBOT command failure.         */
  MSDBOT_XFER_ERROR = -3,           /**< MSDBOT transfer error.          */
} MSDBOT_Status_TypeDef;

/*** MSDBOT Function prototypes ***/

#if defined(USB_HOST)

int MSDBOT_Init(USBH_Ep_TypeDef *out, USBH_Ep_TypeDef *in);
int MSDBOT_Xfer(void* cbw, void* data);

#endif

#ifdef __cplusplus
}
#endif

/** @} (end group Msd) */
/** @} (end group Drivers) */

#endif /* __MSDBOT_H */
