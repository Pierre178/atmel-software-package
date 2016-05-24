/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2016, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \page crypto_qspi_aesb Crypto QSPI AESB Example
 *
 * \section Purpose
 *
 * This example aims to protect electronic data with Advanced Encryption Standard
 * Bridge (AESB).
 *
 * \section Requirements
 *
 * This package can be used on SAMA5D2x Xplained board.
 *
 * \section Description
 *
 * This example shows how to configure AESB to protect electronic data. The
 * Automatic Bridge mode, when the AESB block is connected between the system bus
 * and QSPI, provides automatic encryption/decryption to/from QSPI without 
 * any action on the part of the user.
 *
 * \section Usage
 *
 *  -# Build the program and download it inside the evaluation board. Please
 *     refer to the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/6421B.pdf">
 *     SAM-BA User Guide</a>, the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *     GNU-Based Software Development</a>
 *     application note or to the
 *     <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">
 *     IAR EWARM User Guide</a>,
 *     depending on your chosen solution.
 *  -# On the computer, open and configure a terminal application (e.g.
 *     HyperTerminal on Microsoft Windows) with these settings:
 *        - 115200 bauds
 *        - 8 data bits
 *        - No parity
 *        - 1 stop bit
 *        - No flow control
 *  -# Start the application. The following traces shall appear on the terminal:
 *     \code
 *      -- Crypto QSPI AESB Example xxx --
 *      -- SAMxxxxx-xx
 *      -- Compiled: xxx xx xxxx xx:xx:xx --
 *      QSPI drivers initialized
 *    \endcode
 * \section References
 * - crypto_qspi_aesb/main.c
 * - qspiflash.c
 */

/**
 * \file
 *
 * This file contains all the specific code for the qspi_xip example.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"
#include "chip.h"
#include "trace.h"
#include "compiler.h"

#include "peripherals/aic.h"
#include "peripherals/pio.h"
#include "peripherals/pmc.h"
#include "peripherals/qspi.h"
#include "peripherals/wdt.h"
#include "peripherals/aesb.h"

#include "memories/qspiflash.h"
#include "misc/console.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** Pins to configure for the application. */
static struct _pin pins_qspi[] = QSPIFLASH_PINS;

/** QSPI serial flash instance. */
static struct _qspiflash flash;

/*----------------------------------------------------------------------------
 *        Global functions
 *----------------------------------------------------------------------------*/

/**
 *  \brief CRYPRO_QSPI_AESB Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	bool verify_failed = 0;
	uint32_t baudrate, idx;
	uint8_t buffer[4096];
	uint8_t buffer_read[4096];

	/* Disable watchdog */
	wdt_disable();

	/* Disable all PIO interrupts */
	pio_reset_all_it();

	/* Configure console */
	board_cfg_console(0);

	/* Output example information */
	console_example_info("QSPI AESB Example");

	board_cfg_pmic();

	/* Enable peripheral clock */
	pmc_enable_peripheral(ID_AESB);
	/* Perform a software-triggered hardware reset of the AES interface */
	aesb_swrst();

	printf("-I- Configure AESB in automatic bridge mode: AES CTR selected\n\r");
	/* Enable AESB automatic bridge mode */
	aesb_configure( AESB_MR_AAHB
					| AESB_MR_DUALBUFF_ACTIVE
					| AESB_MR_PROCDLY(0)
					| AESB_MR_SMOD_AUTO_START
					| AESB_MR_OPMOD_CTR
					| AESB_MR_CKEY_PASSWD
					);

	/* Initialize the QSPI and serial flash */
	pio_configure(pins_qspi, ARRAY_SIZE(pins_qspi));

	trace_debug("Initializing QSPI drivers...\n\r");
	qspi_initialize(QSPIFLASH_ADDR);
	trace_debug("QSPI drivers initialized.\n\r");

	baudrate = qspi_set_baudrate(QSPIFLASH_ADDR, QSPIFLASH_BAUDRATE);
	trace_debug("QSPI baudrate set to %uHz\r\n", (unsigned)baudrate);

	printf("Configuring QSPI Flash...\n\r");
	if (!qspiflash_configure(&flash, QSPIFLASH_ADDR)) {
		trace_fatal("Configure QSPI Flash failed!\n\r");
	}
	printf("QSPI Flash configured.\n\r");

	printf("-I- Enable QSPI AESB IP scope (0x900000000-0x980000000)\n\r");
	qspiflash_use_aesb(&flash, true);

	/* Write 64 word buffer with walking bit pattern (0x01, 0x02, ...) */
	for (idx = 0; idx < ARRAY_SIZE(buffer); idx++){
		buffer[idx] = 1 << (idx % 8);
	}

	if (!qspiflash_erase_block(&flash, 0, 4096)) {
		trace_fatal("QSPI Flash block erase failed!\n\r");
	}

	printf("-I- Writing to address of QSPI AESB IP scope, the data is encrypted automatically\n\r");
	if (!qspiflash_write(&flash, 0, buffer, ARRAY_SIZE(buffer))) {
		trace_fatal("QSPI Flash writing failed!\n\r");
	}

	printf("-I- Read from address of QSPI AESB IP scope\n\r");
	memset(buffer_read, 0, ARRAY_SIZE(buffer_read));
	if (!qspiflash_read(&flash, 0, buffer_read, ARRAY_SIZE(buffer_read))) {
			trace_fatal("Read the code from QSPI Flash failed!\n\r");
	}
	verify_failed = false;
	printf("-I- Read and verify data from address of AESB IP scope\r\n");
	for (idx = 0; idx < ARRAY_SIZE(buffer_read); idx++) {
		if (buffer_read[idx] != buffer[idx]) {
			verify_failed = true;
			printf("-E- Data does not match at 0x%x (0x%02x != 0x%02x)\n\r",
				   (unsigned)(buffer_read + idx), buffer_read[idx], buffer[idx]);
			break;
		}
	}
	if (!verify_failed) {
		printf("\r\n-I- As expected, it automatically decrypts the data read from the target slave before putting it on the system bus\r\n");
	}
	
	printf("\r\n-I- Read data from address outside of AESB IP scope. This test is expeted to fail.\r\n");
	
	qspiflash_use_aesb(&flash, false);

	printf("-I- Read buffer without using AESB IP scope\n\r");
	memset(buffer_read, 0, ARRAY_SIZE(buffer_read));
	if (!qspiflash_read(&flash, 0, buffer_read, ARRAY_SIZE(buffer_read))) {
			trace_fatal("Read the code from QSPI Flash failed!\n\r");
	}
	
	verify_failed = false;
	printf("-I- Read and verify data from address 0xD00000000 \r\n");
	for (idx = 0; idx < ARRAY_SIZE(buffer_read); idx++) {
		if (buffer_read[idx] != buffer[idx]) {
			verify_failed = true;
			printf("-E- Data does not match at 0x%x (0x%02x != 0x%02x)\n\r",
				   (unsigned)(buffer_read + idx), buffer_read[idx], buffer[idx]);
			break;
		}
	}
	if (verify_failed) {
		printf("\r\n-I- As expected, data cannot be decrypted from address outside of AESB IP scope\r\n");
	}
	while (1);
}

