/* $Id$
**
** Copyright 1998 Sidney Cammeresi
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <boot.h>
#include <sys/stat.h>
#include <blt/syscall.h>
#include <blt/namer.h>
#include <blt/error.h>
#include <blt/hash.h>
#include <blt/libsyms.h>
#include <blt/vfs.h>
#include "vfs-int.h"
#include "path.h"
#include "shm.h"

#define FG_GREEN  "\033[32m"
#define FG_RED    "\033[34m"
#define FG_WHITE  "\033[37m"

int vfs_port;
struct fs_type *fs_drivers;
struct superblock *mount_list = NULL;
hashtable_t *conn_table;

//extern struct fs_type rootfs, bootfs, portalfs;
extern struct fs_type rootfs, bootfs;
extern void __libc_init_console ();

int fs_register (struct fs_type *driver)
{
	struct fs_type *p;

	if (fs_drivers == NULL)
	{
		fs_drivers = driver;
		fs_drivers->next = NULL;
		return 0;
	}
	else
	{
		p = fs_drivers;
		while (p->next != NULL)
		{
			if (!strcmp (p->name, driver->name))
				return 1;
			p = p->next;
		}
		p->next = driver;
		driver->next = NULL;
		return 0;
	}
}

struct superblock *fs_find (const char *node)
{
	int len, bestlen;
	struct superblock *super, *best;

	super = mount_list;
	len = bestlen = 0;
	best = NULL;
	while (super != NULL)
	{
		if (!strncmp (super->sb_dir, node, len = strlen (super->sb_dir)))
			if (len > bestlen)
			{
				best = super;
				bestlen = len;
			}
		super = super->sb_next;
	}
	return best;
}

vfs_res_t *vfs_openconn (int rport, int area)
{
	int i, private_port;
	vfs_res_t *res;
	struct client *client;

	res = malloc (sizeof (vfs_res_t));
	res->status = VFS_OK;
	res->errno = 0;
	private_port = port_create (rport);
	port_slave (vfs_port, private_port);
	res->data[0] = private_port;

	client = malloc (sizeof (struct client));
	client->in = rport;
	client->out = private_port;
	client->filename_area = area;
	area_clone (area, 0, (void **) &client->nameptr, 0);
	for (i = 0; i < MAX_FDS; i++)
		client->ioctx.fdarray.ofile[i] = NULL;
	hashtable_insert (conn_table, client->in, client, sizeof (struct client));

#ifdef VFS_DEBUG
	printf ("vfs_openconn: from port %d, assigned port %d, area %d "
		"mapped to %p\n", rport, private_port, area, client->nameptr);
#endif
	return res;
}

vfs_res_t *vfs_scroll_area (struct client *client, vfs_cmd_t *vc)
{
	struct ofile *ofile;
	vfs_res_t *res;

	res = malloc (sizeof (vfs_res_t));
	ofile = client->ioctx.fdarray.ofile[vc->data[0]];
	shm_write (ofile, vc->data[1], 0, ofile->length, &res->data[0],
		&res->data[1]);
	res->status = VFS_OK;
	res->errno = 0;
	return res;
}

int vfs_mount (const char *dir, char *type, int flags, void *data)
{
	int res;
	static int rootmounted = 0;
	struct fs_type *driver;
	struct superblock *super;

#ifdef VFS_DEBUG
	printf ("vfs_mount %s %s\n", dir, type);
#endif

	/* find filesystem driver */
	driver = fs_drivers;
	while (driver != NULL)
	{
		if (!strcmp (driver->name, type))
			break;
		else
			driver = driver->next;
	}
	if (driver == NULL)
	{
		printf ("vfs: no driver\n");
		return 1;
	}
	super = malloc (sizeof (struct superblock));
	super->sb_vops = driver->t_vops;

	super->sb_dir = malloc (strlen (dir) + 1);
	strcpy (super->sb_dir, dir);

	/* verify mount point existence */
	if (!rootmounted)
	{
		rootmounted = 1;
		super->sb_dev = "none";
	}
	else
	{
		super->sb_dev = "none";
	}

	super->sb_vnode_cache = hashtable_new (0.75);
	super->sb_next = NULL;
	res = driver->t_vops->mount (super, data, 1);
	if (!res)
	{
		if (mount_list != NULL)
			super->sb_next = mount_list;
		mount_list = super;
#ifdef VFS_DEBUG
		printf ("vfs: mounted type %s on %s from %s\n", type,
			super->sb_dir, super->sb_dev);
#endif
	}
	//if (!strcmp (dir, "/boot")) for (;;) ;
	return res;
}

int vfs_mkdir (const char *dir, mode_t mode)
{
	const char *s;
	int res;
	struct superblock *super;
	struct vnode *vnode;

#ifdef VFS_DEBUG
	printf ("vfs_mkdir %s %d\n", dir, mode);
#endif

	/* find superblock and check if mkdir supported */
	super = fs_find (dir);
	if (super->sb_vops->mkdir == NULL)
		return ENOSYS;

	/* XXX dispatch to root vnode for now */
	vnode = super->sb_root;
	s = dir + strlen (super->sb_dir);
	res = super->sb_vops->mkdir (vnode, s, mode);
	return res;
}

vfs_res_t *vfs_opendir (struct client *client, vfs_cmd_t *vc)
{
	const char *dir;
	int i;
	vfs_res_t *res;
	struct ofile *ofile;
	struct superblock *super;
	struct vnode *vnode;

	dir = client->nameptr + vc->data[0];
	res = malloc (sizeof (vfs_res_t));
#ifdef VFS_DEBUG
	printf ("vfs_opendir %s\n", dir);
#endif

	/* get a file descriptor */
	for (i = 0; i < MAX_FDS; i++)
		if (client->ioctx.fdarray.ofile[i] == NULL)
			break;
	if (i == MAX_FDS)
	{
		res->status = VFS_ERROR;
		res->errno = EMFILE;
		return res;
	}

	/* is opendir supported? */
	super = fs_find (dir);
	if (super == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOENT;
		return res;
	}
	if (super->sb_vops->opendir == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOSYS;
		return res;
	}

	/* find directory's vnode */
	if (!strcmp (dir, super->sb_dir))
		vnode = super->sb_root;
	else
	{
		printf ("UNTESTED\n");
		vnode = super->sb_vops->walk (super->sb_root, dir +
			strlen (super->sb_dir));
	}

	/* stat here to check for directory */

	/* call filesystem's opendir and read the directory */
	ofile = client->ioctx.fdarray.ofile[i] = malloc (sizeof (struct ofile));
	ofile->o_vnode = vnode;
	ofile->area = area_clone (vc->data[1], 0, &ofile->dataptr, 0);
	ofile->offset = vc->data[2];
	ofile->length = vc->data[3];
	res->errno = super->sb_vops->opendir (vnode, &ofile->o_cookie);
	res->status = res->errno ? VFS_ERROR : VFS_OK;
	res->data[0] = i;
	return res;
}

vfs_res_t *vfs_closedir (struct client *client, vfs_cmd_t *vc)
{
	vfs_res_t *res;
	struct ofile *ofile;
	struct vnode *vnode;

#ifdef VFS_DEBUG
	printf ("vfs_closedir %d\n", vc->data[0]);
#endif
	res = malloc (sizeof (vfs_res_t));

	/* is file descriptor valid? */
	if ((ofile = client->ioctx.fdarray.ofile[vc->data[0]]) == NULL)
	{
		res->errno = EBADF;
		res->status = VFS_ERROR;
		return res;
	}

	/* is closedir supported? */
	vnode = ofile->o_vnode;
	if (vnode->v_sb->sb_vops->closedir == NULL)
	{
		res->errno = ENOSYS;
		res->status = VFS_ERROR;
		return res;
	}

	/* call filesystem */
	vnode->v_sb->sb_vops->closedir (vnode, ofile->o_cookie);
	if (!res->errno && (vnode->v_sb->sb_vops->free_dircookie != NULL))
		vnode->v_sb->sb_vops->free_dircookie (ofile->o_cookie);
	free (ofile);
	client->ioctx.fdarray.ofile[vc->data[0]] = NULL;

	res->status = VFS_OK;
	res->errno = 0;
	return res;
}

vfs_res_t *vfs_open (struct client *client, vfs_cmd_t *vc)
{
	int i;
	char *path;
	vfs_res_t *res;
	struct ofile *ofile;
	struct superblock *super;
	struct vnode *vnode;

	res = malloc (sizeof (vfs_res_t));
	path = client->nameptr + vc->data[0];
#ifdef VFS_DEBUG
	printf ("vfs_open %s\n", path);
#endif

	/* get a file descriptor */
	for (i = 0; i < MAX_FDS; i++)
		if (client->ioctx.fdarray.ofile[i] == NULL)
			break;
	if (i == MAX_FDS)
	{
		res->status = VFS_ERROR;
		res->errno = EMFILE;
		return res;
	}

	/* is open supported? */
	super = fs_find (path);
	if (super == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOENT;
		return res;
	}
	if (super->sb_vops->open == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOSYS;
		return res;
	}

	/* find file's vnode */
	vnode = super->sb_vops->walk (super->sb_root, path +
		strlen (super->sb_dir) + 1);
	if (vnode == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOENT;
		return res;
	}

	/* call filesystem */
	ofile = client->ioctx.fdarray.ofile[i] = malloc (sizeof (struct ofile));
	ofile->o_vnode = vnode;
	ofile->area = area_clone (vc->data[1], 0, &ofile->dataptr, 0);
	ofile->offset = vc->data[2];
	ofile->length = vc->data[3];
	res->errno = super->sb_vops->open (vnode, &ofile->o_cookie);
	res->status = res->errno ? VFS_ERROR : VFS_OK;
	res->data[0] = i;
	return res;
}

vfs_res_t *vfs_close (struct client *client, vfs_cmd_t *vc)
{
	printf ("vfs_close\n");
	return NULL;
}

vfs_res_t *vfs_rstat (struct client *client, vfs_cmd_t *vc)
{
	char *path;
	vfs_res_t *res;
	struct superblock *super;
	struct vnode *vnode;
	struct stat *buf;

#ifdef VFS_DEBUG
	printf ("vfs_rstat\n");
#endif
	res = malloc (sizeof (vfs_res_t) + sizeof (struct stat));
	buf = (void *) res + sizeof (vfs_res_t);
	path = client->nameptr + vc->data[0];

	/* is stat supported? */
	super = fs_find (path);
	if (super == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOENT;
		return res;
	}
	if (super->sb_vops->rstat == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOSYS;
		return res;
	}

	/* find file's vnode */
	vnode = super->sb_vops->walk (super->sb_root, path +
		strlen (super->sb_dir) + 1);
	if (vnode == NULL)
	{
		res->status = VFS_ERROR;
		res->errno = ENOENT;
		return res;
	}

	/* call filesystem */
	res->errno = super->sb_vops->rstat (vnode, buf);
	res->status = res->errno ? VFS_ERROR : VFS_OK;
	return res;
}

int vfs_main (volatile int *ready)
{
	int nh, size;
	msg_hdr_t msg, reply;
	vfs_cmd_t vc;
	vfs_res_t *res;
	struct client *client;

	/* open a connection to the console */
	__libc_init_console ();

	/* get a public port and register ourself with the namer */
	vfs_port = port_create (0);
	nh = namer_newhandle ();
	(void) namer_register (nh, vfs_port, "vfs");
	namer_delhandle (nh);

	/* say hello */
#ifdef VFS_DEBUG
	printf ("vfs: " FG_GREEN "listener ready" FG_WHITE " (port %d)\n",
		vfs_port);
#endif

	/* initialise structures */
	fs_drivers = NULL;
	conn_table = hashtable_new (0.75);
	res = NULL;
	client = NULL;

	/* mount the root and boot filesystems */
	fs_register (&rootfs);
	fs_register (&bootfs);
	//fs_register (&portalfs);
	vfs_mount ("/", "rootfs", 0, NULL);
	vfs_mkdir ("/boot", 755);
	vfs_mount ("/boot", "bootfs", 0, NULL);
	//vfs_mkdir ("/portal", 755);
	//vfs_mount ("/portal", "portalfs", 0, NULL);
	*ready = 1;

	for (;;)
	{
		/*
		 * listen for commands.  all ports will be slaved to the one
		 * we just created, so we only need to listen on one port.
		 */
		msg.src = 0;
		msg.dst = vfs_port;
		msg.data = &vc;
		msg.size = sizeof (vc);
		port_recv (&msg);

		if (vc.cmd != VFS_OPENCONN)
			client = hashtable_lookup (conn_table, msg.src, NULL);
		size = sizeof (vfs_res_t);

		switch (vc.cmd)
		{
			case VFS_OPENCONN:
				res = vfs_openconn (msg.src, vc.data[0]);
				break;

			case VFS_SCROLL_AREA:
				res = vfs_scroll_area (client, &vc);
				break;

			case VFS_OPENDIR:
				res = vfs_opendir (client, &vc);
				if (res->status == VFS_OK)
					shm_write_dir (client->ioctx.fdarray.ofile[res->data[0]],
						0, 0, vc.data[3], &res->data[1], &res->data[2]);
				break;

			case VFS_CLOSEDIR:
				res = vfs_closedir (client, &vc);
				break;

			case VFS_OPEN:
				res = vfs_open (client, &vc);
				if (res->status == VFS_OK)
					shm_write (client->ioctx.fdarray.ofile[res->data[0]],
						0, 0, vc.data[3], &res->data[1], &res->data[2]);
				break;

			case VFS_CLOSE:
				res = vfs_close (client, &vc);
				break;

			case VFS_RSTAT:
				res = vfs_rstat (client, &vc);
				size = sizeof (vfs_res_t) + sizeof (struct stat);
				break;
		}

		if (res != NULL)
		{
			reply.src = (vc.cmd == VFS_OPENCONN) ? vfs_port : client->out;
			reply.dst = msg.src;
			reply.data = res;
			reply.size = size;
			port_send (&reply);
			free (res);
		}
	}

	/* not reached */
	return 0;
}

int main (void)
{
	volatile int ready = 0;

	thr_create (vfs_main, (int *) &ready, "vfs");
	while (!ready) ;
	return 0;
}

