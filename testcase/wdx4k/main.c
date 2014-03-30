#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>

#include "GlobalVar.h"
#include "Sock.h"
#include "Packet.h"
#include "Xrfunit.h"

#include "rs232.h"
#include "DeviceI2C.h"
#include "DeviceSPI.h"


/* local variable , initialize in function init_globals() */
pthread_t sock_listen_tid;
pthread_t sock_recv_tid;//sock_send_tid;

/* global variable , initialize in function init_globals() */
volatile unsigned char process_exit;
struct XInstrument instrument;
enum XStatus instrument_status = Idle;

/* initialize in function init_mutex()*/
pthread_mutex_t queue_mutex;
//pthread_mutex_t queue_outgoing_mutex;
//pthread_cond_t  queue_outgoing_cond;

extern void init_globals();
extern void init_mutex();

int main()
{
    init_globals(); /*initialize global variable defined in main.c*/

    if(!init_instrument()){
        printf("initialize instrument error \n");
        exit(1);
    }

    init_iic();
    init_spi();
    init_mutex();
    init_queue(); //ptr_queue = NULL;

	if( init_sock() == -1 ){
	    printf("Error occurs init_sock() \n");
	    exit(1);
    }

    if( pthread_create(&sock_listen_tid,NULL,sock_listen_thread,NULL) != 0 ){
        printf("Error occurs pthread_create(sock_listen_tid) \n");
        exit(1);
    }



    while(!process_exit){
        //printf("Main Thread \n");
        //if(net_status == CONNECTED) heart_beat++;

        packet_process();
        instrument_update(); //do device IO

        usleep(10*1000);     //sleep 0.01 seconds
    }
    sleep(1); // wait for sock_listen_thread, galil_uart_thread

    if(pthread_join(sock_listen_tid,NULL) !=0 ){
        printf("Error occurs pthread_join(sock_listen_tid) \n");
     } /*
    if(pthread_join(galil_uart_thread,NULL) !=0 ){
        printf("Error occurs pthread_join(galil_uart_thread) \n");
     } */

    free_pack_queue(queue);
    free_pack_queue(queue_outgoing);

    if(pthread_mutex_destroy(&queue_mutex) == EBUSY)
        printf("Error occurs _mutex_destroy(&queue_mutex) \n");

    /*
    if(pthread_mutex_destroy(&queue_outgoing_mutex) == EBUSY)
    	 printf("Error occurs _mutex_destroy(&queue_outgoing_mutex) \n");

    if(pthread_cond_destroy(&queue_outgoing_cond) == EBUSY)
    	 printf("Error occurs _cond_destroy(&queue_outgoing_cond) \n");
	*/
	return 0;
}

/*init global var in main.c*/
void init_globals()
{
    printf("Starting init_globals() \n");
    process_exit    = 0;

    sock_listen_tid = 0;

    sock_recv_tid   =0;
    //sock_send_tid   =0;
    heart_beat = 0;

}

void init_mutex()
{
    if(pthread_mutex_init(&queue_mutex,NULL) != 0)
        printf("ERROR _mutex_init(&queue_mutex) \n");
    /*
    if(pthread_mutex_init(&queue_outgoing_mutex,NULL) != 0)
    	printf("ERROR _mutex_init(&queue_outgoing_mutex) \n");

    if(pthread_cond_init(&queue_outgoing_cond,NULL) != 0)
       printf("ERROR _cond_init(&queue_outgoing_cond) \n");

	*/
}

/* printf_errno("init_sock()",errno); */
void printf_errno(const char* func_name,int error)
{
    printf("Error %s errno = %d",func_name,error);
}

void print_ids(const char* name)
{
    pid_t      pid = getpid();
    pthread_t tid = pthread_self();
    printf("PRINT THREAD INFO : %s pid %u, tid %u \n",name,(unsigned int)pid,(unsigned int)tid);
}

void* test_thread(void* t)
{
    union FloatByte test;
    //test.real = 50.00f;
    test.byte[0]=0x00;
    test.byte[1]=0x00;
    test.byte[2]=0x48;
    test.byte[3]=0x42;
    int i;
    /*
    for(i=0;i<4;i++)
    	printf("test.byte[%d]=%x \n",i,test.byte[i]); */
    printf("test.real = %f \n",test.real);
    printf("this is test_thread \n");
    return ((void*)0);
}

