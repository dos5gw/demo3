#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "Packet.h"
#include "GlobalVar.h"

msg_packet* queue = NULL;
msg_packet* queue_outgoing = NULL;


void init_queue()
{
	queue = NULL;
	queue_outgoing = NULL;

    /*
    queue = (msg_packet*)malloc(sizeof(msg_packet));
    if(queue == NULL){
        printf("ERROR malloc Queue \n");
        assert(0);
    }*/
}

/*
  cmd  : 1, BYTE
  dest : 4, UNIT32
  len  : 4, UNIT32
  index: 4, BYTE
  PTR
  */
int init_queue_node(struct MSG_PACKET* p,BYTE* data, int data_size) // *data, include msg head
{
    if(p == NULL) return -1;

    if(data == NULL){
    	p->cmd  = 0x00; //1
    	p->dest = 0;	//2
    	p->len  = 0;	//3
    	p->index= 0x00; //4
    	p->pdata= NULL; //5
    	p->next = NULL; //6
    	return 0;

    }else{
    	p->cmd = *(data); //1
    	memcpy(&(p->dest),data+1,4);
    	memcpy(&(p->len),data+1,4);
    	p->index= *(data+9);//4
    	if((p->pdata = malloc(data_size - 9)) == NULL) return -1;
    	memcpy(p->pdata,data + 9,data_size - 9);
    	p->next = NULL; //6
    	return 0;
    }

}

//add_to_pack_queue(buf,recv_sum);
int add_to_pack_queue(BYTE* src, int src_size) // *src include msg head
{
    //BYTE* SOCK_RECV_BUFF[MAX_RECV_MSG_LEN];
    //volatile int recv_buff_in_ptr,recv_buff_out_ptr;

	msg_packet* p_tail;
    msg_packet* p_node = (msg_packet*)malloc(sizeof(struct MSG_PACKET));
    if(p_node == NULL)
    {
    	printf("Error occurs add_to_pack_queue() \n");
    	return -1;
    }
    if(init_queue_node(p_node,src,src_size) == -1)
    {
    	printf("Error occurs init_queue_node \n");
    	return -1;
    }

    pthread_mutex_lock(&queue_mutex);   //------ add new node to tail
    p_tail = queue;
    if(p_tail == NULL)
    {
    	queue = p_node;
    }
    else
    {
    	while(p_tail->next != NULL)
    	{
    		p_tail = p_tail->next;
    	}
    	p_tail->next = p_node;
    }
    pthread_mutex_unlock(&queue_mutex); //------

    //void printf_current_node(msg_packet* p_node);
    return 0;
}

msg_packet* get_from_pack_queue(void)
{
    msg_packet* p_head = NULL;

    pthread_mutex_lock(&queue_mutex);  //------ get node from head
    if(queue != NULL) {
    	p_head = queue;
    	queue = queue->next;
    }
    pthread_mutex_unlock(&queue_mutex);//-----

    return p_head;
}

msg_packet* get_from_outgoing_queue(void)
{
	msg_packet* p_head = NULL;
	if(queue_outgoing != NULL) {
	    	p_head = queue_outgoing;
	    	queue_outgoing = queue_outgoing->next;
	}
	return p_head;
}

//printf_all_node(void)
void printf_all_node()
{
    pthread_mutex_lock(&queue_mutex); //---
    msg_packet* link = queue;
    int i = 0;
    while(link){
    	printf("node[%2d] ",i++);
    	printf_current_node(link);
    	link = link->next;
    }

    pthread_mutex_unlock(&queue_mutex);//---
    return;
}

void printf_current_node(msg_packet* p_node)
{
    printf(" len=%d index=%u \n",p_node->len,p_node->index);
    return;
}

void free_pack_queue(msg_packet* p) //free_pack_queue(queue)
{
	msg_packet* temp;
    while(p){
        temp = p->next;
        free(p->pdata);
        free(p);
        p = temp;
    }
}

void process_incoming()
{
	//pthread_mutex_lock(&queue_mutex);  //------ get node from head
	msg_packet* p_first_node = get_from_pack_queue();
	//pthread_mutex_unlock(&queue_mutex);//-----

	if(p_first_node == NULL) return;
	printf("get_from_pack_queue() ");
	printf_current_node(p_first_node);

	BYTE* data = p_first_node->pdata;
	unsigned char index = *data;
	printf("packet index = %u \n",index);
	switch(index){
		case 0x10:
			printf("process_sampling_param() \n");
	    	//process_sampling_param(data); //Xrfunit.h
	    	break;
		case 0x11:
			printf("process_analysis_param() \n");
	    	//process_analysis_param(data);//Xrfunit.h
	    	break;
		case 0x12:
			break;
		case 0x13:
			printf("set_instrument_env() \n");
			//set_instrument_env(data);    //Xrfunit.h
			break;
		case 0x14:
			printf("instrument_change_mode() \n");
			//instrument_change_mode(data);//Xrfunit.h
			break;
		case 0x15:
			printf("set_instrument_env_adv() \n");
			//set_instrument_env_adv(data); //Xrfunit.h
			break;
		case 0x16: //upload data, Q, environment param
			printf("upload_env_data() \n");
			//upload_env_data();
			break;
		default:
			printf("unknown packet index \n");
			break;
	}

	free(p_first_node->pdata);
	free(p_first_node);
}

void process_outgoing()
{

}

void packet_process(void)
{
	process_incoming();
	process_outgoing();

}


