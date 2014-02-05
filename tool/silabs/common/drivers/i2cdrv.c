/***************************************************************************//**
 * @file
 * @brief I2C0 poll based driver for master mode operation on DK.
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

#include <stddef.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "i2cdrv.h"

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initalize basic I2C master mode driver for use on the DK.
 *
 * @details
 *   This driver only supports master mode, single bus-master. In addition
 *   to configuring the EFM32 I2C peripheral module, it also configures DK
 *   specific setup in order to use the I2C bus.
 *
 * @param[in] init
 *   Pointer to I2C initialization structure.
 ******************************************************************************/
void I2CDRV_Init(const I2C_Init_TypeDef *init)
{
  int i;

  BSP_PeripheralAccess(BSP_I2C, true);
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_I2C0, true);

  /* Use location 3: SDA - Pin D14, SCL - Pin D15 */
  /* Output value must be set to 1 to not drive lines low... We set */
  /* SCL first, to ensure it is high before changing SDA. */
  GPIO_PinModeSet(gpioPortD, 15, gpioModeWiredAnd, 1);
  GPIO_PinModeSet(gpioPortD, 14, gpioModeWiredAnd, 1);

  /* In some situations (after a reset during an I2C transfer), the slave */
  /* device may be left in an unknown state. Send 9 clock pulses just in case. */
  for (i = 0; i < 9; i++)
  {
    /*
     * TBD: Seems to be clocking at appr 80kHz-120kHz depending on compiler
     * optimization when running at 14MHz. A bit high for standard mode devices,
     * but DK only has fast mode devices. Need however to add some time
     * measurement in order to not be dependable on frequency and code executed.
     */
    GPIO_PinModeSet(gpioPortD, 15, gpioModeWiredAnd, 0);
    GPIO_PinModeSet(gpioPortD, 15, gpioModeWiredAnd, 1);
  }

  /* Enable pins at location 3 (which is used on the DK) */
  I2C0->ROUTE = I2C_ROUTE_SDAPEN |
                I2C_ROUTE_SCLPEN |
                (3 << _I2C_ROUTE_LOCATION_SHIFT);

  I2C_Init(I2C0, init);
}


/***************************************************************************//**
 * @brief
 *   Perform I2C transfer.
 *
 * @details
 *   This driver only supports master mode, single bus-master. It does not
 *   return until the transfer is complete, polling for completion.
 *
 * @param[in] seq
 *   Pointer to sequence structure defining the I2C transfer to take place. The
 *   referenced structure must exist until the transfer has fully completed.
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2CDRV_Transfer(I2C_TransferSeq_TypeDef *seq)
{
  I2C_TransferReturn_TypeDef ret;
  uint32_t                   timeout = 300000;
  /* Do a polled transfer */
  ret = I2C_TransferInit(I2C0, seq);
  while (ret == i2cTransferInProgress && timeout--)
  {
    ret = I2C_Transfer(I2C0);
  }

  return(ret);
}
