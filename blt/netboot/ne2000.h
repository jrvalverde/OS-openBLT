/* $Id$
**
** Copyright 1998 Brian J. Swetland
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions, and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions, and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __NE2000_SHARED_CODE_BASE
#define __NE2000_SHARED_CODE_BASE
typedef unsigned int uint;
typedef struct snic snic;
typedef struct nic_stat nic_stat;
typedef struct buffer_header buffer_header;
typedef struct packet_buffer packet_buffer;
typedef struct nic_error_stat nic_error_stat;

/* internal implementation procedures */
int nic_probe(int addr);
int nic_dump_prom(snic *nic, unsigned char *prom);
void nic_overrun(snic *nic);
void nic_tx(snic *nic);
void nic_tx_err(snic *nic);
void nic_rx(snic *nic);
void nic_counters(snic *nic);
void nic_get_header(snic *nic, uint page, buffer_header *header);
int nic_send(snic *nic, uint buf);
void nic_block_input(snic *nic, unsigned char *buf, uint len, uint offset);
void nic_block_output(snic *nic, unsigned char *buf, uint len, uint page);

#define PSTART			0x20	/* if NE2000 is byte length */
#define PSTOP			0x40
#define PSTARTW			0x40	/* if NE2000 is wordlength */
#define PSTOPW			0x80
#define MAX_LOAD		12	/* maximum services per IRQ request*/
#define MAX_RX			10	/* maximum packets recieve per call*/
#define MIN_LENGTH		60	/* minimum length for packet data */
#define MAX_LENGTH		1500	/* maximum length for packet data area */
#define TIMEOUT_DMAMATCH	40	/* for nic_block_input() */
#define TIMEOUT_TX		40

/* DP8390 NIC Registers*/
#define COMMAND			0x00
#define STATUS			COMMAND+0
#define PHYSICAL		COMMAND+1	/* page 1 */
#define MULTICAST		COMMAND+8	/* page 1 */
#define	PAGESTART		COMMAND+1	/* page 0 */
#define PAGESTOP		COMMAND+2
#define BOUNDARY		COMMAND+3
#define TRANSMITSTATUS		COMMAND+4
#define TRANSMITPAGE		COMMAND+4
#define TRANSMITBYTECOUNT0	COMMAND+5
#define NCR			COMMAND+5
#define TRANSMITBYTECOUNT1	COMMAND+6
#define INTERRUPTSTATUS		COMMAND+7
#define CURRENT			COMMAND+7	/* page 1 */
#define REMOTESTARTADDRESS0	COMMAND+8
#define CRDMA0			COMMAND+8
#define REMOTESTARTADDRESS1	COMMAND+9
#define CRDMA1			COMMAND+9
#define REMOTEBYTECOUNT0	COMMAND+10	/* how many bytes we will */
#define REMOTEBYTECOUNT1	COMMAND+11	/* read through remote DMA->IO */
#define RECEIVESTATUS		COMMAND+12
#define RECEIVECONFIGURATION	COMMAND+12
#define TRANSMITCONFIGURATION	COMMAND+13
#define FAE_TALLY		COMMAND+13	/* page 0 */
#define DATACONFIGURATION	COMMAND+14
#define CRC_TALLY		COMMAND+14
#define INTERRUPTMASK		COMMAND+15
#define MISS_PKT_TALLY		COMMAND+15

/* NE2000 specific implementation registers */
#define NE_RESET	0x1f	/* Reset */
#define NE_DATA		0x10	/* Data port (use for PROM) */

#define PAR0			COMMAND+1
#define PAR1			COMMAND+2
#define PAR2			COMMAND+3
#define PAR3			COMMAND+4
#define PAR4			COMMAND+5
#define PAR5			COMMAND+6

/* NIC Commands */
#define NIC_STOP	0x01	/* STOP */
#define NIC_START	0x02	/* START */
#define NIC_PAGE0	0x00
#define	NIC_PAGE1	0x40
#define NIC_PAGE2	0x80
#define NIC_TRANSMIT	0x04	/* Transmit a frame */
#define NIC_REM_READ	0x08	/* Remote Read */
#define NIC_REM_WRITE	0x10	/* Remote Write */
#define NIC_DMA_DISABLE	0x20	/* Disable DMA */

/* Data Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS		0x01	/* Word Trans0x40Configuration Register */
#define DCR_WTS	