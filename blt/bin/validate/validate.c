
#include <stdio.h>
#include <stdlib.h>
#include <blt/syscall.h>
#include <blt/namer.h>

int send_port, recv_port;


void sender(void)
{
	char buffer[32];
	msg_hdr_t mh;

	os_sleep(20);
		
	for(;;){
		mh.src = send_port;
		mh.dst = recv_port;
		mh.size = 32;
		mh.flags = 0;
		mh.data = buffer;
		
		port_send(&mh);
	}
}

void receiver(void)
{
	char buffer[32];
	msg_hdr_t mh;
	
	recv_port = port_create(0);
	os_sleep(20);
	
	for(;;){
		mh.src = 0;
		mh.dst = recv_port;
		mh.size = 32;
		mh.flags = 0;
		mh.data = buffer;
		
		port_recv(&mh);
	}
}

int main (int argc, char **argv)
{
	int s,c;
	__libc_init_console ();

#if 0
	send_port = port_create(0);
	os_thread(sender);
	receiver();
#endif
	
	printf("sem_test: starting\n");
	for(c=0;c<1000000;c++){
		if(!(c % 100000)) printf("sem_test: %dth semaphore\n",c);
		if((s = sem_create(1)) < 1) {
			printf("sem_test: failed (in create) - iteration %d\n",c);
			return 1;
		}
		if(sem_destroy(s)){
			printf("sem_test: failed (in destroy) - iteration %d\n",c);
			return 1;
		}
	}
	printf("sem_test: passed\n");

	printf("port_test: starting\n");
	for(c=0;c<1000000;c++){
		if(!(c % 100000)) printf("port_test: %dth port\n",c);
		if((s = port_create(0)) < 1) {
			printf("port_test: failed (in create) - iteration %d\n",c);
			return 1;
		}
		if(port_destroy(s)){
			printf("port_test: failed (in destroy) - iteration %d\n",c);
			return 1;
		}
	}
	printf("port_test: passed\n");
	return 0;
	
}

