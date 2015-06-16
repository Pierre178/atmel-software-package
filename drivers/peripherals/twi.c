/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2015, Atmel Corporation
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

/** \addtogroup twi_module Working with TWI
 * \section Purpose
 * The TWI driver provides the interface to configure and use the TWI
 * peripheral.
 *
 * \section Usage
 * <ul>
 * <li> Configures a TWI peripheral to operate in master mode, at the given
 * frequency (in Hz) using TWI_Configure(). </li>
 * <li> Sends a STOP condition on the TWI using twi_stop().</li>
 * <li> Starts a read operation on the TWI bus with the specified slave using
 * twi_start_read(). Data must then be read using twi_read_byte() whenever
 * a byte is available (poll using twi_is_byte_received()).</li>
 * <li> Starts a write operation on the TWI to access the selected slave using
 * twi_start_write(). A byte of data must be provided to start the write;
 * other bytes are written next.</li>
 * <li> Sends a byte of data to one of the TWI slaves on the bus using twi_write_byte().
 * This function must be called once before twi_start_write() with the first byte of data
 * to send, then it shall be called repeatedly after that to send the remaining bytes.</li>
 * <li> Check if a byte has been received and can be read on the given TWI
 * peripheral using twi_is_byte_received().<
 * Check if a byte has been sent using twi_byte_sent().</li>
 * <li> Check if the current transmission is complete (the STOP has been sent)
 * using twi_is_transfer_complete().</li>
 * <li> Enables & disable the selected interrupts sources on a TWI peripheral
 * using twi_enable_it() and twi_enable_it().</li>
 * <li> Get current status register of the given TWI peripheral using
 * twi_get_status(). Get current status register of the given TWI peripheral, but
 * masking interrupt sources which are not currently enabled using
 * twi_get_masked_status().</li>
 * </ul>
 * For more accurate information, please look at the TWI section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref twi.c\n
 * \ref twi.h.\n
*/
/*@{*/
/*@}*/

/**
 * \file
 *
 * Implementation of Two Wire Interface (TWI).
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"
#include "peripherals/twi.h"
#include "peripherals/pmc.h"
#include "trace.h"

#include <stddef.h>
#include <assert.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Configures a TWI peripheral to operate in master mode, at the given
 * frequency (in Hz). The duty cycle of the TWI clock is set to 50%.
 * \param twi  Pointer to an Twi instance.
 * \param twi_clock  Desired TWI clock frequency.
 */
void twi_configure_master(Twi * pTwi, uint32_t twi_clock)
{
	uint32_t ck_div, cl_div, ok, clock;
	uint32_t id = get_twi_id_from_addr(pTwi);

	trace_debug("twi_configure_master()\n\r");
	assert(pTwi);
	assert(id != ID_PERIPH_COUNT);
	/* SVEN: TWI Slave Mode Enabled */
	pTwi->TWI_CR = TWI_CR_SVEN;
	/* Reset the TWI */
	pTwi->TWI_CR = TWI_CR_SWRST;
	pTwi->TWI_RHR;
	/* TWI Slave Mode Disabled, TWI Master Mode Disabled. */
	pTwi->TWI_CR = TWI_CR_SVDIS;
	pTwi->TWI_CR = TWI_CR_MSDIS;
	/* Set master mode */
	pTwi->TWI_CR = TWI_CR_MSEN;
	clock = pmc_get_peripheral_clock(id);
	/* Configure clock */
	ck_div = 0; ok = 0;
	while (!ok) {
		cl_div = ((clock / (2 * twi_clock)) - 3) >> ck_div;
		if (cl_div <= 255)
			ok = 1;
		else
			ck_div++;
	}
	assert(ck_div < 8);
	trace_debug("Using CKDIV = %u and CLDIV/CHDIV = %u\n\r",
		    (unsigned int)ck_div, (unsigned int)cl_div);
	pTwi->TWI_CWGR = 0;
	pTwi->TWI_CWGR = TWI_CWGR_CKDIV(ck_div) | TWI_CWGR_CHDIV(cl_div) |
		TWI_CWGR_CLDIV(cl_div);
}

/**
 * \brief Configures a TWI peripheral to operate in slave mode.
 * \param twi  Pointer to an Twi instance.
 * \param slaveAddress Slave address.
 */
void twi_configure_slave(Twi * pTwi, uint8_t slave_address)
{
	uint32_t i;

	trace_debug("twi_configure_slave()\n\r");
	assert(pTwi);
	/* TWI software reset */
	pTwi->TWI_CR = TWI_CR_SWRST;
	pTwi->TWI_RHR;
	/* Wait at least 10 ms */
	for (i = 0; i < 1000000; i++) ;
	/* TWI Slave Mode Disabled, TWI Master Mode Disabled */
	pTwi->TWI_CR = TWI_CR_SVDIS | TWI_CR_MSDIS;
	/* Configure slave address. */
	pTwi->TWI_SMR = 0;
	pTwi->TWI_SMR = TWI_SMR_SADR(slave_address);
	/* SVEN: TWI Slave Mode Enabled */
	pTwi->TWI_CR = TWI_CR_SVEN;
	/* Wait at least 10 ms */
	for (i = 0; i < 1000000; i++) ;
	assert((pTwi->TWI_CR & TWI_CR_SVDIS) != TWI_CR_SVDIS);
}

/**
 * \brief Sends a STOP condition on the TWI.
 * \param twi  Pointer to an Twi instance.
 */
void twi_stop(Twi * pTwi)
{
	assert(pTwi != NULL);
	pTwi->TWI_CR = TWI_CR_STOP;
}

/**
 * \brief Starts a read operation on the TWI bus with the specified slave, it returns
 * immediately. Data must then be read using twi_read_byte() whenever a byte is
 * available (poll using twi_is_byte_received()).
 * \param twi  Pointer to an Twi instance.
 * \param address  Slave address on the bus.
 * \param iaddress  Optional internal address bytes.
 * \param isize  Number of internal address bytes.
 */
void twi_start_read(Twi * pTwi, uint8_t address, uint32_t iaddress, uint8_t isize)
{
	assert(pTwi != NULL);
	assert((address & 0x80) == 0);
	assert((iaddress & 0xFF000000) == 0);
	assert(isize < 4);
	/* Set slave address and number of internal address bytes. */
	pTwi->TWI_MMR = 0;
	pTwi->TWI_MMR = (isize << 8) | TWI_MMR_MREAD | (address << 16);
	/* Set internal address bytes */
	pTwi->TWI_IADR = 0;
	pTwi->TWI_IADR = iaddress;
	/* Send START condition */
	pTwi->TWI_CR = TWI_CR_START;
}

/**
 * \brief Reads a byte from the TWI bus. The read operation must have been started
 * using twi_start_read() and a byte must be available (check with twi_is_byte_received()).
 * \param twi  Pointer to an Twi instance.
 * \return byte read.
 */
uint8_t twi_read_byte(Twi * pTwi)
{
	assert(pTwi != NULL);
	return pTwi->TWI_RHR;
}

/**
 * \brief Sends a byte of data to one of the TWI slaves on the bus.
 * \note This function must be called once before twi_start_write() with
 * the first byte of data  to send, then it shall be called repeatedly
 * after that to send the remaining bytes.
 * \param twi  Pointer to an Twi instance.
 * \param byte  Byte to send.
 */
void twi_write_byte(Twi * pTwi, uint8_t byte)
{
	assert(pTwi != NULL);
	pTwi->TWI_THR = byte;
}

/**
 * \brief Starts a write operation on the TWI to access the selected slave, then
 *  returns immediately. A byte of data must be provided to start the write;
 * other bytes are written next.
 * after that to send the remaining bytes.
 * \param twi  Pointer to an Twi instance.
 * \param address  Address of slave to acccess on the bus.
 * \param iaddress  Optional slave internal address.
 * \param isize  Number of internal address bytes.
 * \param byte  First byte to send.
 */
void twi_start_write(Twi * pTwi, uint8_t address, uint32_t iaddress, uint8_t isize, uint8_t byte)
{
	assert(pTwi != NULL);
	assert((address & 0x80) == 0);
	assert((iaddress & 0xFF000000) == 0);
	assert(isize < 4);
	/* Set slave address and number of internal address bytes. */
	pTwi->TWI_MMR = 0;
	pTwi->TWI_MMR = (isize << 8) | (address << 16);
	/* Set internal address bytes. */
	pTwi->TWI_IADR = 0;
	pTwi->TWI_IADR = iaddress;
	/* Write first byte to send. */
	twi_write_byte(pTwi, byte);
}

/**
 * \brief Check if a byte have been receiced from TWI.
 * \param twi  Pointer to an Twi instance.
 * \return 1 if a byte has been received and can be read on the given TWI
 * peripheral; otherwise, returns 0. This function resets the status register.
 */
uint8_t twi_is_byte_received(Twi * pTwi)
{
	assert(pTwi != NULL);
	return ((pTwi->TWI_SR & TWI_SR_RXRDY) == TWI_SR_RXRDY);
}

/**
 * \brief Check if a byte have been sent to TWI.
 * \param twi  Pointer to an Twi instance.
 * \return 1 if a byte has been sent  so another one can be stored for
 * transmission; otherwise returns 0. This function clears the status register.
 */
uint8_t twi_byte_sent(Twi * pTwi)
{
	assert(pTwi != NULL);
	return ((pTwi->TWI_SR & TWI_SR_TXRDY) == TWI_SR_TXRDY);
}

/**
 * \brief Check if current transmission is complet.
 * \param twi  Pointer to an Twi instance.
 * \return  1 if the current transmission is complete (the STOP has been sent);
 * otherwise returns 0.
 */
uint8_t twi_is_transfer_complete(Twi * pTwi)
{
	assert(pTwi != NULL);
	return ((pTwi->TWI_SR & TWI_SR_TXCOMP) == TWI_SR_TXCOMP);
}

/**
 * \brief Enables the selected interrupts sources on a TWI peripheral.
 * \param twi  Pointer to an Twi instance.
 * \param sources  Bitwise OR of selected interrupt sources.
 */
void twi_enable_it(Twi * pTwi, uint32_t sources)
{
	assert(pTwi != NULL);
	assert((sources & 0xFFFFF088) == 0);
	pTwi->TWI_IER = sources;
}

/**
 * \brief Disables the selected interrupts sources on a TWI peripheral.
 * \param twi  Pointer to an Twi instance.
 * \param sources  Bitwise OR of selected interrupt sources.
 */
void twi_disable_it(Twi * pTwi, uint32_t sources)
{
	assert(pTwi != NULL);
	assert((sources & 0xFFFFF088) == 0);
	pTwi->TWI_IDR = sources;
}

/**
 * \brief Get the current status register of the given TWI peripheral.
 * \note This resets the internal value of the status register, so further
 * read may yield different values.
 * \param twi  Pointer to an Twi instance.
 * \return  TWI status register.
 */
uint32_t twi_get_status(Twi * pTwi)
{
	assert(pTwi != NULL);
	return pTwi->TWI_SR;
}

/**
 * \brief Returns the current status register of the given TWI peripheral, but
 * masking interrupt sources which are not currently enabled.
 * \note This resets the internal value of the status register, so further
 * read may yield different values.
 * \param twi  Pointer to an Twi instance.
 */
uint32_t twi_get_masked_status(Twi * pTwi)
{
	uint32_t status;
	assert(pTwi != NULL);
	status = pTwi->TWI_SR;
	status &= pTwi->TWI_IMR;
	return status;
}

/**
 * \brief  Sends a STOP condition. STOP Condition is sent just after completing
 *  the current byte transmission in master read mode.
 * \param twi  Pointer to an Twi instance.
 */
void twi_send_stop_condition(Twi * pTwi)
{
	assert(pTwi != NULL);
	pTwi->TWI_CR |= TWI_CR_STOP;
}

#ifdef FIFO_ENABLED
void twi_fifo_configure(Twi* twi, uint8_t tx_thres,
			uint8_t rx_thres,
			uint32_t ready_modes)
{
	/* Disable TWI master and slave mode and activate FIFO */
	twi->TWI_CR = TWI_CR_MSDIS | TWI_CR_SVDIS | TWI_CR_FIFOEN;

	/* Configure FIFO */
	twi->TWI_FMR = TWI_FMR_TXFTHRES(tx_thres) | TWI_FMR_RXFTHRES(rx_thres)
		| ready_modes;
}

uint32_t twi_fifo_rx_size(Twi *twi)
{
	return (twi->TWI_FLR & TWI_FLR_RXFL_Msk) >> TWI_FLR_RXFL_Pos;
}

uint32_t twi_fifo_tx_size(Twi *twi)
{
	return (twi->TWI_FLR & TWI_FLR_TXFL_Msk) >> TWI_FLR_TXFL_Pos;
}

uint32_t twi_write_stream(Twi *twi, uint32_t addr, const void *stream, uint32_t len)
{
	const uint8_t* buffer = stream;
	uint32_t left = len;

	twi->TWI_CR = TWI_CR_MSEN | TWI_CR_SVDIS | TWI_CR_ACMEN;
	twi->TWI_MMR = TWI_MMR_DADR(addr);
	twi->TWI_ACR = TWI_ACR_DATAL(len) | TWI_ACR_DIR;

	while (left > 0) {
		if ((twi->TWI_SR & TWI_SR_TXRDY) == 0) continue;

		/* Get FIFO free size (int octet) and clamp it */
		uint32_t buf_size = TWI_FIFO_DEPTH - twi_fifo_tx_size(twi);
		buf_size = buf_size > left ? left : buf_size;

		/* Fill the FIFO as must as possible */
		while (buf_size > sizeof(uint32_t)) {
			twi->TWI_THR = *(uint32_t*)buffer;
			buffer += sizeof(uint32_t);
			left -= sizeof(uint32_t);
			buf_size -= sizeof(uint32_t);
		}
		while (buf_size >= sizeof(uint8_t)) {
			twi->TWI_THR = *(uint8_t*)buffer;
			buffer += sizeof(uint16_t);
			left -= sizeof(uint16_t);
			buf_size -= sizeof(uint32_t);
		}

	}
	return len - left;
}

#endif