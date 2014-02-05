/**************************************************************************//**
 * @file
 * @brief EFM32GG_DK3750, TFT Initialization and setup for Direct Drive mode
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
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
#include <stdbool.h>
#include "em_device.h"
#include "em_gpio.h"
#include "em_ebi.h"
#include "bsp.h"
#include "tftdirect.h"
/* DMD init from dmd_ssd2119_direct.c */
#include "glib/glib.h"

/** Flag to indicate that we need to initialize once if starting up in */
static bool runOnce = true;

/**************************************************************************//**
 * @brief  GPIO configuration for TFT direct drive
 *****************************************************************************/
void TFT_DirectGPIOConfig(void)
{
  /* EBI TFT DCLK/Dot Clock */
  GPIO_PinModeSet(gpioPortA, 8, gpioModePushPull, 0);
  /* EBI TFT DATAEN */
  GPIO_PinModeSet(gpioPortA, 9, gpioModePushPull, 0);
  /* EBI TFT VSYNC  */
  GPIO_PinModeSet(gpioPortA, 10, gpioModePushPull, 0);
  /* EBI TFT HSYNC */
  GPIO_PinModeSet(gpioPortA, 11, gpioModePushPull, 0);
}


/**************************************************************************//**
 * @brief  TFT initialize or reinitialize
 *         Assumes EBI has been configured correctly in BSP_Init(BSP_INIT_DK_EBI)
 *
 * @param[in] tftInit Pointer to EBI TFT initialization structure
 *
 * @return true if we should redraw into buffer, false if BC has control
 *         over display
 *****************************************************************************/
bool TFT_DirectInit(const EBI_TFTInit_TypeDef *tftInit)
{
  bool     ret;
  uint32_t i, freq;
  EMSTATUS stat;

  /* If we are in BC_UIF_AEM_EFM state, we can redraw graphics */
  if (BSP_RegisterRead(&BC_REGISTER->UIF_AEM) == BC_UIF_AEM_EFM)
  {
    /* If we're not BC_ARB_CTRL_EBI state, we need to reconfigure display controller */
    if ((BSP_RegisterRead(&BC_REGISTER->ARB_CTRL) != BC_ARB_CTRL_EBI) || runOnce)
    {
      /* Enable SSD2119 Serial Port Interface */
      BSP_PeripheralAccess(BSP_TFT, true);

      /* Enable EBI mode of operation on SSD2119 controller */
      BSP_DisplayControl(BSP_Display_EBI);
      BSP_DisplayControl(BSP_Display_ResetAssert);
      BSP_DisplayControl(BSP_Display_PowerDisable);
      freq = SystemCoreClockGet();
      for (i = 0; i < (freq / 100); i++)
      {
        __NOP();
      }
      /* Configure display for Direct Drive "Mode Generic" + 3-wire SPI mode */
      BSP_DisplayControl(BSP_Display_ModeGeneric);
      BSP_DisplayControl(BSP_Display_PowerEnable);
      BSP_DisplayControl(BSP_Display_ResetRelease);

      /* Configure GPIO for EBI and TFT */
      TFT_DirectGPIOConfig();

      /* Initialize display */
      stat = DMD_init(0);
      if (DMD_OK == stat) stat = DMD_selectFramebuffer((void*)EBI_BankAddress(EBI_BANK2));
      if (DMD_OK != stat) while (1) ;

      /* Configure EBI TFT direct drive */
      EBI_TFTInit(tftInit);

      runOnce = false;
    }
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}
