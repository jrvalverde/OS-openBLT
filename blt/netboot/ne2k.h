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
#ifndef __NE2000_INTERFACE
#define __NE2000_INTERFACE
#ifndef __NE2000_SHARED_CODE_BASE
typedef unsigned int uint;
typedef struct snic snic;
typedef struct nic_stat nic_stat;
typedef struct buffer_header buffer_header;
typedef struct packet_buffer packet_buffer;
typedef struct nic_error_stat nic_error_stat;
#endif /* NE2000_SHARED_CODE_BASE */

#define LEN_ADDR		6
#define MAX_TX			2	/* be careful with this (dual buffers) */
#define MAX_PAGESPERPACKET	6
#define TXPAGES			(MAX_TX*MAX_PAGESPERPACKET)
#define LEN_PROM		16
#define LEN_HEADER		14

#ifdef __CPP_BINDING
extern "C" {
#endif

/* external interface */
int nic_detect(int given);
int nic_init(snic* nic, int addr, unsigned char *prom, unsigned char *manual);
void nic_register_notify(snic *nic, void (*newnotify)(packet_buffer *newpacket));
void nic_start(snic *nic, int promiscuous);
void nic_stop(snic *nic);
void nic_isr(snic *nic);
nic_stat nic_get_stats(snic *nic);
void nic_stat_clear(nic_stat *that);
int nic_send_packet(snic *nic, packet_buffer*);

/* Your implementation */
packet_buffer *alloc_buffer(uint size);
void free_buffer(packet_buffer *ptr);

#ifdef __CPP_BINDING
};
#endif

struct buffer_header {
	unsigned char status;
	unsigned char next;
	unsigned short count;	/* length of header and packet */
};

struct nic_error_stat {
	long frame_alignment, crc, missed_packets;
	long rx, rx_size, rx_dropped, rx_fifo, rx_overruns;
	long tx_collisions;
	long tx_aborts, tx_carrier, tx_fifo, tx_heartbeat, tx_window;
};

struct nic_stat {
	long rx_packets;
	long tx_buffered, tx_packets;
	nic_error_stat errors;
};

struct packet_buffer {
	uint len;
	uint page;
	unsigned char *ptr;
};

struct snic {
	int iobase;	/* NULL if uninitialized */
	int pstart, pstop, wordlength, current_page;
	nic_stat stat;
	void (*notify)(packet_buffer *newpacket);
	packet_buffer tx_packet[MAX_TX], *last_tx;
	char busy, send;
};

#endif	/* __NE2000_INTERFACE */
