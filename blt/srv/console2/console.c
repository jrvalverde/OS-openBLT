/* $Id$
**
** Copyright 1998 Brian J. Swetland
** All rights reserved.
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

#include <blt/syscall.h>
#include <blt/error.h>
#include <blt/namer.h>
#include <blt/conio.h>
#include <blt/qsem.h>
#include <i386/io.h>
#include <string.h>

#include "vt100.h"

int console_port;
int send_port;
int input_port;
volatile int ready = 0;

#define CLEAR "\033[2J"
#define FG_BLACK  "\033[30m"
#define FG_BLUE   "\033[31m"
#define FG_GREEN  "\033[32m"
#define FG_CYAN   "\033[33m"
#define FG_RED    "\033[34m"
#define FG_PURPLE "\033[35m"
#define FG_BROWN  "\033[36m"
#define FG_WHITE  "\033[37m"

#define BG_BLACK  "\033[40m"
#define BG_BLUE   "\033[41m"
#define BG_GREEN  "\033[42m"
#define BG_CYAN   "\033[43m"
#define BG_RED    "\033[44m"
#define BG_PURPLE "\033[45m"
#define BG_BROWN  "\033[46m"
#define BG_WHITE  "\033[47m"   


#define MONOx

#ifdef MONO
#define SCREEN 0xB0000
#else
#define SCREEN 0xB8000
#endif
void *screen = (void *) SCREEN;

void movecursor (int x, int y)
{
	int offset;

	offset = 80 * y + x;
	outb (0xe, 0x3d4);
	outb (offset / 256, 0x3d5);
	outb (0xf, 0x3d4);
	outb (offset % 256, 0x3d5);
}

struct virtscreen con[10];
struct virtscreen statbar;
struct virtscreen *active;

void move_cursor(struct virtscreen *cur)
{
	if(cur == active) movecursor(cur->xpos,cur->ypos);
}

void vprintf(struct virtscreen *vscr, char *fmt, ...);
void printf(char *fmt, ...);

void keypress(int key)
{
	qsem_acquire(active->input_lock);
	if (active->input_len < sizeof (active->input))
	{
		active->input[active->input_len++] = key;
		sem_acquire(active->lock);
		char_to_virtscreen(active, key);
		sem_release(active->lock);
		qsem_release(active->len_lock);
	}
	qsem_release(active->input_lock);
}

void vputs(struct virtscreen *vscr, char *s)
{
    sem_acquire(vscr->lock);
	while(*s) {
		char_to_virtscreen(vscr, *s);
        if(*s == '\n') char_to_virtscreen(vscr, '\r');
		s++;
	}
    sem_release(vscr->lock);
}


void printf(char *fmt, ...)
{
    static char line[128];
    va_list pvar;
    va_start(pvar,fmt);
    va_snprintf(line,128,fmt,pvar);
    line[127]=0;
    va_end(pvar);
    vputs(active,line);
}

void vprintf(struct virtscreen *vscr, char *fmt, ...)
{
    static char line[128];
    va_list pvar;
    va_start(pvar,fmt);
    va_snprintf(line,128,fmt,pvar);
    line[127]=0;
    va_end(pvar);
    vputs(vscr,line);
}
                                    
void status(void)
{
	vputs(&statbar,FG_WHITE BG_BLUE CLEAR "### OpenBLT Console mkII ###");
}

#define ESC 27
#define BS 8
#define TAB 9
#define CR 13
#define LF 10

char ScanTable [] =  {' ', ESC, '1', '2', '3', '4', '5', '6', '7', '8',
                      '9', '0', '-', '=', BS,  TAB, 'q', 'w', 'e', 'r',
                      't', 'y', 'u', 'i', 'o', 'p', '[', ']', LF,  ' ',
                      'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
                      '\'', '`', ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n',
                      'm', ',', '.', '/', ' ', ' ', ' ', ' ', ' '};
char ShiftTable [] = {' ', ESC, '!', '@', '#', '$', '%', '^', '&', '*',
                      '(', ')', '_', '+', ' ', ' ', 'Q', 'W', 'E', 'R',
                      'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', LF,  ' ',
                      'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
                      '\"', '~', ' ', '|', 'Z', 'X', 'C', 'V', 'B', 'N',
                      'M', '<', '>', '?', ' ', ' ', ' ', ' ', ' '};
#define LSHIFT 42
#define RSHIFT 54
#define CONTROL 0x1d
#define ALT 0x38

#define F1  0x3b
#define F2  0x3c
#define F3  0x3d
#define F4  0x3e
#define F5  0x3f
#define F6  0x40
#define F7  0x41
#define F8  0x42
#define F9  0x43
#define F10 0x44

void movecursor (int x, int y);
void keypress(int key);

void save(struct virtscreen *vscr)
{
    sem_acquire(vscr->lock);
    if(vscr->data != vscr->back) {
        memcpy(vscr->back,vscr->data,vscr->num_bytes);
        vscr->data = vscr->back;
    }
    sem_release(vscr->lock);
}

void load(struct virtscreen *vscr)
{
    sem_acquire(vscr->lock);
    if(vscr->data == vscr->back) {
        vscr->data = screen;
        memcpy(vscr->data,vscr->back,vscr->num_bytes);
    }
    active = vscr;
    movecursor(vscr->xpos,vscr->ypos);
    sem_release(vscr->lock);
    status();
}

void function(int number)
{
    save(active);
    active = &con[number];
    load(active);
}

void keyboard_irq_thread(void)
{
    int shift = 0;    
	int control = 0;
	int alt = 0;
	
    int key;

    send_port = port_create(console_port);
    os_handle_irq(1);
	
    for(;;) {
        os_sleep_irq();
#ifdef MULTI
        while(inb(0x64) & 0x01) {
#endif
            key = inb(0x60);
            if(alt && (key == 1)) {
                save(active);
                os_debug();
                load(active);
                alt = 0;
                continue;
			}
			
            switch(key){
            case F1:
            case F2:
            case F3:
            case F4:
            case F5:
            case F6:
            case F7:
            case F8:
            case F9:
            case F10:
                function(key - F1);
                break;
                
			case ALT:
				alt = 1;
				break;
			case ALT | 0x80:
				alt = 0;
				break;
			case CONTROL:
				control = 1;
				break;
			case CONTROL | 0x80:
				control = 0;
				break;
            case LSHIFT:
            case RSHIFT:
                shift = 1;
                break;
            case LSHIFT | 0x80:
            case RSHIFT | 0x80:
                shift = 0;
                break;
            default:
                if(key & 0x80){
                        /* break */
                } else {
                    if(key < 59){
						if(control){
							key = ScanTable[key];
							if(key >= 'a' && key <= 'z'){
								keypress(key - 'a' + 1);
							}
						} else {
							key = shift ? ShiftTable[key] : ScanTable[key];
							keypress(key);
						} 
                    }
                }
            }
#ifdef MULTI
        }
#endif
    }

}

#if 0
void test(void)
{
	int a0, a1;
	int i;
	unsigned char *ac0, *ac1;
	a0 = area_create(0x8000,0,(void **) &ac0, 0);
	a1 = area_clone(a0,0,(void **) &ac1, 0);

	vprintf(active,"area id %U @ 0x%x\r\n",a0,(uint32) ac0);
	vprintf(active,"area id %U @ 0x%x\r\n",a1,(uint32) ac1);

	for(i=0;i<0x8000;i++){
		ac0[i] = (i % 256);
		if(ac0[i] != ac1[i]) vprintf(active,"barfo @ %x\r\n",i);
	}
	vprintf(active,"success.\r\n");

}
#endif

void console_thread(void)
{
    int l;
    char data[257];
    msg_hdr_t msg;
        
    msg.data = data;
    msg.dst = console_port;
    msg.size = 256;
#ifdef CONSOLE_DEBUG
    vprintf(active, "console: " FG_GREEN "listener ready" FG_WHITE " (port %d)\n",console_port);
#endif
    
    while((l = port_recv(&msg)) > 0){
        data[l] = 0;
        vputs(active, data);
    }
    vprintf(active, "console: output listener has left the building\n");
    os_terminate(0);
}

void input_thread(void)
{
    int i, j, nh, len;
    char data[32];
    msg_hdr_t msg, reply;
    
	input_port = port_create(0);
	nh = namer_newhandle();    
	namer_register(nh, input_port,"console_input");
	namer_delhandle(nh);    

#ifdef CONSOLE_DEBUG
    vprintf(active, "console: " FG_GREEN "input listener ready" FG_WHITE
		" (port %d)\n",input_port);
#endif
	ready = 1;

	for (;;)
	{
		msg.src = 0;
		msg.dst = input_port;
		msg.data = data;
		msg.size = sizeof (data);
		port_recv (&msg);

		len = *((char *) msg.data);
		if (len < 1)
			continue;

		qsem_acquire (active->len_lock);
		qsem_acquire (active->input_lock);
		data[0] = active->input[0];
		for (i = 0; i < (active->input_len - 1); i++)
			active->input[i] = active->input[i + 1];
		active->input_len--;
		qsem_release (active->input_lock);

		reply.src = input_port;
		reply.dst = msg.src;
		reply.data = data;
		reply.size = 1;
		port_send(&reply);
    }
    vprintf(active, "console: input listener has left the building\n");
    os_terminate(0);
}

int console_main(void)
{
    int err,nh,i;    
	area_create(0x2000, 0, &screen, AREA_PHYSMAP);
    
    console_port = port_create(0);
    nh = namer_newhandle();    
    err = namer_register(nh,console_port,"console");
    namer_delhandle(nh);    

	init_virtscreen(&statbar, 1, 80);
    statbar.back = statbar.data;
	statbar.data = (unsigned short *) (((uint32) screen) + 80*24*2);    
    statbar.lock = sem_create(1);
    
    for(i=0;i<10;i++){
        init_virtscreen(&con[i], 24, 80);
        con[i].back = con[i].data;
        con[i].lock = sem_create(1);
		con[i].input_len = 0;
		con[i].input_lock = qsem_create (1);
		con[i].len_lock = qsem_create (0);
        vputs(&con[i],CLEAR);
    }
    load(&con[0]);
	
    status();
    if(err) vprintf(active,"console: the namer hates us\n");
    else
#ifdef CONSOLE_DEBUG
	    vprintf(active,"console: " FG_GREEN "online." FG_WHITE "\n");
#else
        ;
#endif
    
	os_thread(keyboard_irq_thread);
	os_thread(input_thread);
    console_thread();
    
    return 0;
}

int main(void)
{
	os_thread (console_main);
	while (!ready) ;
	return 0;
}

