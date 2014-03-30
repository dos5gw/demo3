#ifndef SOCK_H
#define SOCK_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define LISTENING_PORT 4999
#define RECV_BUFFER_LEN 1024
#define SEND_BUFFER_LEN 1024
#define MAX_RECV_MSG_LEN 4096
#define MAX_SEND_MSG_LEN 4096

#define MAXINTERFACES 16
#define MAX_ENV_PARAM_LEN 35

extern struct MSG_PACKET* queue;
extern struct MSG_PACKET* queue_outgoing; //Packet.c

enum NET_STATUS{
    CONNECTED,
    DISCONNECTED,
    RETRY,
};

int init_sock(void);
int clean_sock(void);

int check_buff_head(const BYTE* buf,int len);

int add_to_recv_buffers(BYTE* src,int src_size);
int add_to_send_buffers(BYTE* src,int src_size);
int get_from_recv_buff();
int get_recv_buff_len();
int send_data(int socket_fd, BYTE* pdata, int size);

/*thread function */
//void* proc_recv_buffer(void*);
void* sock_listen_thread(void*);
void* sock_recv_thread(void*);
void* sock_send_thread(void*);

int upload_sample_data();
int upload_env_data();

/* */
void print_connect_info(struct sockaddr_in* addr);
void print_local_info(const struct sockaddr_in* addr);

int sending_msg_buff(char* pbuff,BYTE cmd,struct sockaddr_in* paddr,int length,BYTE index);
int upload_env_data();

#endif
