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
unsigned char bootblock[] = {
	0xeb, 0x03, 0x00, 0x00, 0x00, 0xfa, 0x33, 0xc0,
	0x8e, 0xd0, 0xbc, 0x00, 0x7c, 0xe8, 0xc4, 0x01,
	0x36, 0x0f, 0x01, 0x16, 0x4f, 0x7d, 0x0f, 0x20,
	0xc0, 0x0c, 0x01, 0x0f, 0x22, 0xc0, 0x66, 0xea,
	0x26, 0x7c, 0x00, 0x00, 0x08, 0x00, 0xbb, 0x10,
	0x00, 0x8e, 0xdb, 0x8e, 0xc3, 0x8e, 0xd3, 0x24,
	0xfe, 0x0f, 0x22, 0xc0, 0xea, 0x39, 0x00, 0xc0,
	0x07, 0x33, 0xc0, 0x8e, 0xc0, 0x8e, 0xd8, 0x8e,
	0xd0, 0x66, 0xbf, 0x00, 0x00, 0x10, 0x00, 0xbb,
	0x02, 0x00, 0x2e, 0x8b, 0x0e, 0x02, 0x00, 0x33,
	0xd2, 0xfb, 0xe8, 0x12, 0x01, 0xfa, 0xe8, 0x4b,
	0x01, 0x66, 0xa3, 0x00, 0x40, 0x66, 0xc7, 0x06,
	0x04, 0x40, 0x00, 0x00, 0x01, 0x00, 0x66, 0x89,
	0xc1, 0x66, 0xc1, 0xe9, 0x0c, 0x66, 0x8b, 0x3e,
	0x04, 0x40, 0x66, 0x81, 0xc1, 0x00, 0x04, 0x00,
	0x00, 0x66, 0x33, 0xc0, 0xf3, 0x67, 0x66, 0xab,
	0x66, 0x8b, 0x1e, 0x04, 0x40, 0x66, 0x8b, 0x0e,
	0x00, 0x40, 0x66, 0xc1, 0xe9, 0x0c, 0x66, 0xb8,
	0x03, 0x00, 0x00, 0x00, 0x66, 0xbf, 0x00, 0x10,
	0x00, 0x00, 0x66, 0x67, 0x89, 0x04, 0x3b, 0x66,
	0x05, 0x00, 0x10, 0x00, 0x00, 0x66, 0x83, 0xc7,
	0x04, 0xe2, 0xef, 0x66, 0x8b, 0x0e, 0x00, 0x40,
	0x66, 0xc1, 0xe9, 0x16, 0x66, 0x41, 0x66, 0xa1,
	0x04, 0x40, 0x66, 0x05, 0x03, 0x10, 0x00, 0x00,
	0x66, 0x33, 0xff, 0x66, 0x67, 0x89, 0x04, 0x3b,
	0x66, 0x67, 0x89, 0x84, 0x3b, 0x00, 0x0f, 0x00,
	0x00, 0x66, 0x05, 0x00, 0x10, 0x00, 0x00, 0x66,
	0x83, 0xc7, 0x04, 0xe2, 0xe6, 0x66, 0xa1, 0x04,
	0x40, 0x66, 0x89, 0xc1, 0x0c, 0x03, 0x66, 0x67,
	0x89, 0x83, 0xfc, 0x0f, 0x00, 0x00, 0x66, 0x67,
	0x8b, 0x1d, 0x74, 0x00, 0x10, 0x00, 0x66, 0x81,
	0xc3, 0x00, 0x10, 0x10, 0x00, 0x66, 0xbd, 0x00,
	0x00, 0x10, 0x00, 0xb0, 0xcf, 0x2e, 0xa2, 0x5d,
	0x01, 0x2e, 0xa2, 0x65, 0x01, 0x2e, 0x0f, 0x01,
	0x16, 0x4f, 0x01, 0x0f, 0x22, 0xd9, 0x66, 0xb8,
	0x01, 0x00, 0x00, 0x80, 0x0f, 0x22, 0xc0, 0x66,
	0xea, 0x27, 0x7d, 0x00, 0x00, 0x08, 0x00, 0x66,
	0xb8, 0x10, 0x00, 0x66, 0x8e, 0xd8, 0x66, 0x8e,
	0xc0, 0x66, 0x8e, 0xe0, 0x66, 0x8e, 0xe8, 0x66,
	0x8e, 0xd0, 0x89, 0xcc, 0x83, 0xec, 0x04, 0xba,
	0x04, 0x7c, 0x00, 0x00, 0x55, 0x52, 0x67, 0xff,
	0x36, 0x00, 0x40, 0xff, 0xd3, 0xeb, 0xfe, 0xff,
	0xff, 0x4f, 0x7d, 0x00, 0x00, 0x00, 0x00, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x9a, 0x8f, 0x00, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x92, 0x8f, 0x00, 0x53,
	0x51, 0xb0, 0x13, 0x28, 0xd8, 0x8b, 0xcb, 0xbb,
	0x00, 0x80, 0xb4, 0x02, 0xcd, 0x13, 0x72, 0x2a,
	0x66, 0xbe, 0x00, 0x80, 0x00, 0x00, 0x66, 0x33,
	0xc9, 0x88, 0xc1, 0xc1, 0xe1, 0x07, 0xf3, 0x67,
	0x66, 0xa5, 0x59, 0x5b, 0x80, 0xf6, 0x01, 0x75,
	0x02, 0xfe, 0xc7, 0xb3, 0x01, 0x32, 0xe4, 0x2b,
	0xc8, 0x7f, 0xcc, 0xb0, 0x0c, 0xba, 0xf2, 0x03,
	0xee, 0xc3, 0xeb, 0xfe, 0xb8, 0x32, 0x31, 0x66,
	0xbb, 0xf0, 0x0f, 0x10, 0x00, 0x67, 0x8b, 0x13,
	0x67, 0x89, 0x03, 0x67, 0x8b, 0x3b, 0x67, 0x89,
	0x13, 0x39, 0xc7, 0x75, 0x09, 0x66, 0x81, 0xc3,
	0x00, 0x10, 0x00, 0x00, 0xeb, 0xe7, 0x66, 0x8b,
	0xc3, 0x66, 0x2d, 0x00, 0x10, 0x00, 0x00, 0x66,
	0x83, 0xc0, 0x10, 0xc3, 0xe8, 0x0f, 0x00, 0x75,
	0x1b, 0xb0, 0xd1, 0xe6, 0x64, 0xe8, 0x06, 0x00,
	0x75, 0x12, 0xb0, 0xdf, 0xe6, 0x60, 0x66, 0xb9,
	0x00, 0x00, 0x02, 0x00, 0xeb, 0x00, 0xe4, 0x64,
	0xa8, 0x02, 0xe0, 0xf8, 0xc3, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x55, 0xaa,
};