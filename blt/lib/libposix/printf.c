/* Copyright 1999, Sidney Cammeresi. All rights reserved.
** Distributed under the terms of the OpenBLT License
*/

#include <stdarg.h>
#include <string.h>
#include <blt/namer.h>
#include <blt/syscall.h>
#include <blt/libsyms.h>

static int __libc_console_public_port, __libc_console_port;

void va_snprintf (char *s, int len, const char *fmt, ...);

void __libc_init_console (void)
{
	__libc_console_public_port = namer_find ("console",1);
	__libc_console_port = port_create (__libc_console_public_port,"console_public_port");
}

weak_alias (_printf, __libc_printf)
weak_alias (_printf, printf)
link_warning (__libc_printf, "warning: __libc_printf is deprecated; use printf instead.")

init_info __init_posix_console = {
	&__libc_init_console,
	2
};

int _printf(char *fmt,...)
{   
	msg_hdr_t kmsg;
	int l;
	char buf[256];

	va_list pvar;
	va_start(pvar,fmt);
	va_snprintf(buf,256,fmt,pvar);
	va_end(pvar);
	kmsg.size = strlen(buf);
	kmsg.data = buf;
	kmsg.src = __libc_console_port;
	kmsg.dst = __libc_console_public_port;
	l = port_send(&kmsg);
	return -1; /* FIXME */
}

