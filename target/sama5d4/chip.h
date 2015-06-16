/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */
/*                                                                              */

#ifndef _CHIP_H_
#define _CHIP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#define __I  volatile	    /**< Defines 'read-only'  permissions */
#else
#define __I  volatile const /**< Defines 'read-only'  permissions */
#endif
#define   __O  volatile	      /**< Defines 'write-only' permissions */
#define   __IO volatile	      /**< Defines 'read/write' permissions */

/* ************************************************************************** */
/*   PERIPHERAL ID DEFINITIONS FOR SAMA5D4x */
/* ************************************************************************** */
/** \addtogroup SAMA5D4x_id Peripheral Ids Definitions */
/*@{*/

#define ID_SYSC         ( 1) /**< \brief System Controller Interrupt, RTC, RSTC, PMC (SYSC) */
#define ID_ARM          ( 2) /**< \brief Performance Monitor Unit (ARM) */
#define ID_PIT          ( 3) /**< \brief Periodic Interval Timer Interrupt (PIT) */
#define ID_WDT          ( 4) /**< \brief Watchdog timer Interrupt (WDT) */
#define ID_PIOD         ( 5) /**< \brief Parallel I/O Controller D (PIOD) */
#define ID_USART0       ( 6) /**< \brief USART 0 (USART0) */
#define ID_USART1       ( 7) /**< \brief USART 1 (USART1) */
#define ID_XDMAC0       ( 8) /**< \brief DMA Controller 0 (XDMAC0) */
#define ID_ICM          ( 9) /**< \brief Integritry Check Monitor (ICM) */
#define ID_CPKCC        (10) /**< \brief Classic Public Key Crypto Controller (CPKCC) */
#define ID_AES          (12) /**< \brief Advanced Encryption Standard (AES) */
#define ID_AESB         (13) /**< \brief AES bridge (AESB) */
#define ID_TDES         (14) /**< \brief Triple Data Encryption Standard (TDES) */
#define ID_SHA          (15) /**< \brief SHA Signature (SHA) */
#define ID_MPDDRC       (16) /**< \brief MPDDR controller (MPDDRC) */
#define ID_MATRIX1      (17) /**< \brief H32MX, 32-bit AHB Matrix (MATRIX1) */
#define ID_MATRIX0      (18) /**< \brief H64MX, 64-bit AHB Matrix (MATRIX0) */
#define ID_VDEC         (19) /**< \brief Video Decoder (VDEC) */
#define ID_MSADCC       (21) /**< \brief Magnetic Stripe converter Controller (MSADCC) */
#define ID_HSMC         (22) /**< \brief Multi-bit ECC Interrupt (HSMC) */
#define ID_PIOA         (23) /**< \brief Parallel I/O Controller A (PIOA) */
#define ID_PIOB         (24) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_PIOC         (25) /**< \brief Parallel I/O Controller C (PIOC) */
#define ID_PIOE         (26) /**< \brief Parallel I/O Controller E (PIOE) */
#define ID_UART0        (27) /**< \brief UART 0 (UART0) */
#define ID_UART1        (28) /**< \brief UART 1 (UART1) */
#define ID_USART2       (29) /**< \brief USART 2 (USART2) */
#define ID_USART3       (30) /**< \brief USART 3 (USART3) */
#define ID_USART4       (31) /**< \brief USART 4 (USART4) */
#define ID_TWI0         (32) /**< \brief Two-Wire Interface 0 (TWI0) */
#define ID_TWI1         (33) /**< \brief Two-Wire Interface 1 (TWI1) */
#define ID_TWI2         (34) /**< \brief Two-Wire Interface 2 (TWI2) */
#define ID_HSMCI0       (35) /**< \brief High Speed Multimedia Card Interface 0 (HSMCI0) */
#define ID_HSMCI1       (36) /**< \brief High Speed Multimedia Card Interface 1 (HSMCI1) */
#define ID_SPI0         (37) /**< \brief Serial Peripheral Interface 0 (SPI0) */
#define ID_SPI1         (38) /**< \brief Serial Peripheral Interface 1 (SPI1) */
#define ID_SPI2         (39) /**< \brief Serial Peripheral Interface 2 (SPI2) */
#define ID_TC0          (40) /**< \brief Timer Counter 0 (ch. 0, 1, 2) (TC0) */
#define ID_TC1          (41) /**< \brief Timer Counter 1 (ch. 3, 4, 5) (TC1) */
#define ID_TC2          (42) /**< \brief Timer Counter 2 (ch. 6, 7, 8) (TC2) */
#define ID_PWM          (43) /**< \brief Pulse Width Modulation Controller (PWM) */
#define ID_ADC          (44) /**< \brief Touch Screen ADC Controller (ADC) */
#define ID_DBGU         (45) /**< \brief Debug Unit Interrupt (DBGU) */
#define ID_UHPHS        (46) /**< \brief USB Host High Speed (UHPHS) */
#define ID_UDPHS        (47) /**< \brief USB Device High Speed (UDPHS) */
#define ID_SSC0         (48) /**< \brief Synchronous Serial Controller 0 (SSC0) */
#define ID_SSC1         (49) /**< \brief Synchronous Serial Controller 1 (SSC1) */
#define ID_XDMAC1       (50) /**< \brief DMA Controller 1 (XDMAC1) */
#define ID_LCDC         (51) /**< \brief LCD Controller (LCDC) */
#define ID_ISI          (52) /**< \brief Camera Interface (ISI) */
#define ID_TRNG         (53) /**< \brief True Random Number Generator (TRNG) */
#define ID_GMAC0        (54) /**< \brief Ethernet MAC 0 (GMAC0) */
#define ID_GMAC1        (55) /**< \brief Ethernet MAC 1 (GMAC1) */
#define ID_IRQ          (56) /**< \brief IRQ Interrupt ID (IRQ) */
#define ID_SFC          (57) /**< \brief Fuse Controller (SFC) */
#define ID_SMD          (61) /**< \brief SMD Soft Modem (SMD) */
#define ID_TWI3         (62) /**< \brief Two-Wire Interface 3 (TWI3) */
#define ID_SFR          (64) /**< \brief Special Function Register  (SFR) */
#define ID_AIC          (65) /**< \brief Advanced Interrupt Controller  (AIC) */
#define ID_SAIC         (66) /**< \brief Secured Advanced Interrupt Controller  (SAIC) */
#define ID_L2CC         (67) /**< \brief L2 Cache Controller  (L2CC) */

#define ID_PERIPH_COUNT (68) /**< \brief Number of peripheral IDs */

/*@}*/

/* ************************************************************************** */
/* INCLUDE FOR SAMA5D4x */
/* ************************************************************************** */

#if defined(CONFIG_SOC_SAMA5D44)
  #include "sama5d44.h"
#else
  #error Library does not support the specified device.
#endif

/* TODO: convert ISI driver to use component instead and remove this include and header file */
#include "instance/instance_isi.h"

/** Frequency of the on-chip slow clock oscillator */
#define SLOW_CLOCK_INT_OSC 32000

/** Frequency of the on-chip main clock oscillator */
#define MAIN_CLOCK_INT_OSC 12000000

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief retrieve TWI ID from its base address
 * \return TWI ID on success, ID_PERIPH_COUNT otherwise
 */
extern uint32_t get_twi_id_from_addr(const Twi* addr);

/**
 * \brief retrieve TWI base address from its ID
 * \return TWI base address on success, 0 otherwise
 */
extern Twi* get_twi_addr_from_id(const uint32_t id);

/**
 *
 */
extern uint32_t get_spi_id_from_addr(const Spi* addr);

extern Spi* get_spi_addr_from_id(const uint32_t id);

extern uint32_t get_uart_id_from_addr(const Uart* addr);

extern uint32_t get_usart_id_from_addr(const Usart* addr);

extern uint32_t get_tc_id_from_addr(const Tc* addr);

/** \brief Returns the matrix on which the given peripheral is connected
 *
 * \param id the Peripheral ID
 * \return a pointer to the Matrix instance
 */
extern Matrix* get_peripheral_matrix(uint32_t id);

/** \brief Returns the clock divider for the given peripheral
 *
 * \param id the Peripheral ID
 * \return the clock divider for the peripheral
 */
extern uint32_t get_peripheral_clock_divider(uint32_t id);

/** \brief Returns the XDMAC interface number for a given peripheral
 *
 * \param id the Peripheral ID
 * \param xdmac the XDMAC controller instance
 * \param transmit a boolean, true for transmit, false for receive
 * \return the XDMAC interface number or 0xff if none
 */
uint8_t get_peripheral_xdma_channel(uint32_t id, Xdmac *xdmac, bool transmit);

/** \brief Checks if a peripheral is usable with a XDMAC controller
 *
 * \param id the Peripheral ID
 * \param xdmac the XDMAC controller instance
 * \return true if the peripheral is usable on the given XDMAC controller,
 * false otherwise
 */
bool is_peripheral_on_xdma_controller(uint32_t id, Xdmac *xdmac);

#ifdef __cplusplus
}
#endif

#endif /* _CHIP_H_ */