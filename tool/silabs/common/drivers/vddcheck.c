/**************************************************************************//**
 * @file
 * @brief VDD Voltage Check, using Voltage Comparator API
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
#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_vcmp.h"
#include "vddcheck.h"

/**************************************************************************//**
 * @brief VCMP initialization routine
 *****************************************************************************/
void VDDCHECK_Init(void)
{
  /* Enable LE peripherals */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable VCMP clock */
  CMU_ClockEnable(cmuClock_VCMP, true);
}

/**************************************************************************//**
 * @brief VCMP deinitialization routine
 *****************************************************************************/
void VDDCHECK_Disable(void)
{
  /* Disable VCMP */
  VCMP_Disable();

  /* Disable clock to VCMP */
  CMU_ClockEnable(cmuClock_VCMP, false);
}

/**************************************************************************//**
 * @brief Check if voltage is higher than indicated
 *
 * @param vdd
 *        The voltage level to compare against.
 * @return
 *        Returns true if voltage is lower, false otherwise
 *****************************************************************************/
bool VDDCHECK_LowVoltage(float vdd)
{
  VCMP_Init_TypeDef vcmp = VCMP_INIT_DEFAULT;

  /* Configure VCMP */
  vcmp.triggerLevel = VCMP_VoltageToLevel(vdd);
  vcmp.warmup       = vcmpWarmTime128Cycles;
  vcmp.lowPowerRef  = false;
  vcmp.enable       = true;

  VCMP_Init(&vcmp);

  /* Delay until warm up ready */
  while (!VCMP_Ready()) ;

  /* If zero result, voltage is lower */
  if (VCMP_VDDHigher()) return false;

  /* Otherwise return false */
  return true;
}
