/* $Id$
**
** Copyright 1999 Sidney Cammeresi
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <blt/libsyms.h>
#include <blt/blkdev.h>
#include <blt/namer.h>
#include <blt/syscall.h>

weak_alias (_blk_open, blk_open)
weak_alias (_blk_close, blk_close)
weak_alias (_blk_read, blk_read)
weak_alias (_blk_write, blk_write)

int __blk_ref;

int _blk_open (const char *name, int flags, blkdev_t **retdev)
{
	char *server;
	int i, nh, len;
	msg_hdr_t mh;
	blkdev_t *dev;
	blktxn_t *txn;
	blkres_t res;

	for (i = 0; name[i] && (name[i] != '/') && i < BLT_MAX_NAME_LENGTH; i++) ;
	server = malloc (i);
	strncpy (server, name, i);
	server[i] = 0;

	dev = malloc (sizeof (blkdev_t));
	nh = namer_newhandle ();
	dev->remote_port = namer_find (nh, server);
	namer_delhandle (nh);

	txn = malloc (len = sizeof (blktxn_t) + strlen (name) - i + 1);
	txn->cmd = BLK_CMD_OPEN;
	strcpy ((char *) (txn + 1), name + i + 1);

	mh.src = dev->local_port = port_create (dev->remote_port, "blkdev");
	mh.dst = dev->remote_port;
	mh.data = txn;
	mh.size = len;
	port_send (&mh);

	mh.src = dev->remote_port;
	mh.dst = dev->local_port;
	mh.data = &res;
	mh.size = sizeof (blkres_t);
	port_recv (&mh);

	if (res.status)
	{
		free (dev);
		*retdev = NULL;
		return res.status;
	}

	dev->blksize = res.data[0];
	dev->devno = res.data[1];
	*retdev = dev;
	return 0;
}

int _blk_close (blkdev_t *dev)
{
	free (dev);
	return 0;
}

int _blk_read (blkdev_t *dev, void *buf, int block, int count)
{
	int len, ret;
	msg_hdr_t mh;
	blktxn_t txn;
	blkres_t *res;

	res = malloc (len = sizeof (blkres_t) + dev->blksize);

	while (count)
	{
		txn.device = dev->devno;
		txn.cmd = BLK_CMD_READ;
		txn.block = block;
		txn.count = count;
		mh.src = dev->local_port;
		mh.dst = dev->remote_port;
		mh.data = &txn;
		mh.size = sizeof (blktxn_t);
		port_send (&mh);

		mh.src = dev->remote_port;
		mh.dst = dev->local_port;
		mh.data = res;
		mh.size = len;
		port_recv (&mh);

		if (res->status)
			goto done;
		memcpy (buf, res + 1, dev->blksize);
		buf = (char *) buf + dev->blksize;
		block++;
		count--;
	}

done:
	ret = res->status;
	free (res);
	return ret;
}

int _blk_write (blkdev_t *dev, const void *buf, int block, int count)
{
	return 0;
}

