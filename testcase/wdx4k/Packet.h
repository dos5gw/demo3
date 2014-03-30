#ifndef PACKET_H
#define PACKET_H

#include "GlobalVar.h"

#define MAX_PACKET_SIZE 128

typedef enum MSG_TYPE
{
	IIC,
	SPI,
	GALIL,
	STATE,
	DEFAULT

}msg_type;

typedef struct MSG_PACKET
{
	BYTE cmd;    //1
	UNIT32 dest; //2
	UNIT32 len;  //3
	BYTE index;  //4
	unsigned char* pdata; //5
	struct MSG_PACKET* next; //6
}msg_packet;

typedef msg_packet* QUEUE;

void* packet_proc_thread(void*);
void packet_process(void);

void init_queue();
//void init_queue_node(struct MSG_PACKET* p);
//void init_queue_node(struct MSG_PACKET* p, BYTE* src, int src_size);
int init_queue_node(struct MSG_PACKET* p,BYTE* data, int data_size);

int add_to_pack_queue(BYTE*, int);
msg_packet* get_from_pack_queue(void);
msg_packet* get_from_outgoing_queue(void);

void printf_all_node(void);
void printf_current_node(msg_packet*);
void free_pack_queue(msg_packet* p);

void instrument_change_mode(BYTE* pdata);

#endif
