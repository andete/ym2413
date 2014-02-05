/***************************************************************************//**
 * @file
 * @brief API for SPI Interface to Micrel KSZ8851SNL ethernet controller.
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
#ifndef __ETHERNET_SPI_H
#define __ETHERNET_SPI_H
#include <stdint.h>

/**************************************************************************//**
* @addtogroup Drivers
* @{
******************************************************************************/
/**************************************************************************//**
* @addtogroup EthSpi
* @{
* The actual communication between the EFM32 chips and the ethernet controller
* is done via SPI using the driver from this file.
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define BAUDRATE_100K        100000             /**< Baudrate 100.000 */
#define ETH_USART_USED       USART1             /**< The used USART */
#define ETH_USART_CLK        cmuClock_USART1    /**< The used USART clock */
#define BOGUS_BYTE           0xFF               /**< Bogus byte used for receiving via SPI */
#define REG_MASK             0x03               /**< Register mask */
#define OPCODE_REG_READ      0x00               /**< Opcode for reading a register */
#define OPCODE_REG_WRITE     0x40               /**< Opcode for writing a register */
#define OPCODE_FIFO_READ     0x80               /**< Opcode for reading from FIFO */
#define OPCODE_FIFO_WRITE    0xC0               /**< Opcode for writing to FIFO */
#define SHIFT_VAL            0x02               /**< Shift value - 2 bits */
#define ADDRESS_MS2B_MASK    0xC0               /**< Most semnificative 2 Bytes of the address mask */
#define ADDRESS_MS2B_POS     0x06               /**< Most semnificative 2 Bytes of the address position */

/* GPIO pins used for SPI communication. */
/*(USART 1, Location #1) */
#define SPI_MOSI_PIN     0                      /**< SPI MOSI Pin */
#define SPI_MOSI_PORT    gpioPortD              /**< SPI MOSI Port */
#define SPI_MISO_PIN     1                      /**< SPI MISO Pin */
#define SPI_MISO_PORT    gpioPortD              /**< SPI MISO Port */
#define SPI_CLK_PIN      2                      /**< SPI CLOCK Pin */
#define SPI_CLK_PORT     gpioPortD              /**< SPI CLOCK Port */
#define SPI_CS_PIN       3                      /**< SPI CHIP SELECT Pin */
#define SPI_CS_PORT      gpioPortD              /**< SPI CHIP SELECT Port */



void ETHSPI_Init(void);
void ETHSPI_ReadRegister(uint8_t reg, int numBytes, void *data);
void ETHSPI_WriteRegister(uint8_t reg, int numBytes, void *data);
void ETHSPI_StartWriteFIFO(void);
void ETHSPI_StartReadFIFO(void);
void ETHSPI_StopFIFO(void);
void ETHSPI_ReadFifoContinue(int numBytes, uint8_t *data);
void ETHSPI_WriteFifoContinue(int numBytes, uint8_t *data);

#ifdef __cplusplus
}
#endif

/** @} (end group EthSpi) */
/** @} (end group Drivers) */

#endif
