#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include "GlobalVar.h"
#include "Sock.h"
#include "Packet.h"

long heart_beat;

/* define local variable , initialize in function init_sock() */
enum NET_STATUS net_status;
static int local_sock_fd, remote_sock_fd;
static struct sockaddr_in local_sock_addr;
static struct sockaddr_in remote_sock_addr;

/* unpacket receive data , initialize in function init_sock()
   BYTE* SOCK_RECV_BUFF[MAX_RECV_MSG_LEN];
get_recv_buff_len    trylock
add_to_recv_buffers _cond_signal
print_recv_buffer   _cond_wait
   */
BYTE SOCK_RECV_BUFF[MAX_RECV_MSG_LEN];
volatile int recv_buff_in_ptr,recv_buff_out_ptr;

/* unpacket send data ,    initialize in function init_sock() */
//BYTE* SOCK_SEND_BUFF[MAX_SEND_MSG_LEN];
BYTE SOCK_SEND_BUFF[MAX_SEND_MSG_LEN];
volatile int send_buff_in_ptr,send_buff_out_ptr;

/*initialize local variable and listen */
int init_sock(void)
{
    //print_ids("init_sock");
    printf("Starting init_sock() \n");

    /*initialize local variable*/
    net_status = DISCONNECTED;
    local_sock_fd = 0;
    remote_sock_fd = 0;
    bzero(&local_sock_addr, sizeof(struct sockaddr_in));
    bzero(&remote_sock_addr,sizeof(struct sockaddr_in));

    /*initialize socket receive and send buffers */
    memset(SOCK_RECV_BUFF,0,MAX_RECV_MSG_LEN);
    recv_buff_in_ptr = 0;
    recv_buff_out_ptr= 0;

    memset(SOCK_SEND_BUFF,0,MAX_RECV_MSG_LEN);
    send_buff_in_ptr = 0;
    send_buff_out_ptr= 0;

    /*initialize socket fd*/
    local_sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(local_sock_fd == -1){
        perror("ERROR socket()");
        return -1;
    }

    /*initialize socket addr*/
    local_sock_addr.sin_family = AF_INET;
    //local_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_sock_addr.sin_addr.s_addr = INADDR_ANY;
    local_sock_addr.sin_port = htons(LISTENING_PORT);

    //perror : Address already in use
    int opt = 1;
    setsockopt(local_sock_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    if( bind(local_sock_fd,(struct sockaddr*)(&local_sock_addr),sizeof(struct sockaddr)) == -1){
        perror("ERROR bind()");
        return -1;
    }

    if(listen(local_sock_fd,5) == -1){
        perror("ERROR linsten()");
        return -1;
    }

    return 0;
}


/*check head /length of buffer
   return value:
    2: heat beat
    1: receive completely
    0: not receive completely
    -1: default ERROR
    -2: not head ERROR */
int check_buff_head(const BYTE* buff, int sum){

    if(sum < 9) {
    	return -2;
    }
    union IntByte msg_addr,msg_len;
	int i;
	for(i = 0; i < 4; i++){ // buff[1] - buff[4]
		msg_addr.byte[i] = *(buff + 1 + i);
	}
	for(i = 0; i < 4; i++){ // buff[5] - buff[8]
		msg_len.byte[i] = *(buff + 5 + i);
	}
	msg_len.real = ntohl(msg_len.real);
	if((msg_len.real > RECV_BUFFER_LEN) || (msg_len.real < 0))
		return -2;

    if(buff[0] == 0x48){ //heart-beat head
    	if((msg_addr.real == 0x00) || (msg_len.real == 0x00)) return 2;
    	else return -2;

    }
    else if(buff[0] == 0x41){ //normal head
    	if(msg_len.real = (sum-9)) return 1;
    	else if(msg_len.real > (sum-9)) return 0;
    	else if(msg_len.real < (sum-9)) return -2;

    }

    return -1; //receive completely


}
/*
void* sock_send_thread(void* arg)
{

	//BYTE* buf = malloc(SEND_BUFFER_LEN*sizeof(unsigned char));
	//memset(buf,0,SEND_BUFFER_LEN);

	while(net_status == CONNECTED)
	{
		pthread_mutex_lock(&queue_outgoing_mutex);
		while(queue_outgoing == NULL)
			pthread_cond_wait(&queue_outgoing_cond,&queue_outgoing_mutex); //unlock mutex and wait for signal
		while(queue_outgoing)
		{
			msg_packet* pnode = get_from_outgoing_queue();
			if(send_data(remote_sock_fd,pnode->pdata,pnode->len) != 0){
				printf("Error occurs send_buff() \n");
			}
		}
		pthread_mutex_unlock(&queue_outgoing_mutex);

	}
	//free(buf);
	printf("sock_recv_thread exit \n");
	return ((void*)0);
} */

void* sock_recv_thread(void* arg){

    // initialize local variable
    int recv_sum = 0;
    int recv_tmp = 0;
    //int length = 0;

    struct timeval timeout = {3,0};
    int select_ret;
    fd_set sel_fds;

    // initialize memory buffer for recv() function
    BYTE* buf = malloc(RECV_BUFFER_LEN*sizeof(unsigned char)); //1024 Bytes
    memset(buf,0,RECV_BUFFER_LEN);

    while(net_status == CONNECTED){

        timeout.tv_sec = 3;
        timeout.tv_usec= 0; // 3.0 seconds

        FD_ZERO(&sel_fds);
        FD_SET(remote_sock_fd,&sel_fds);

        // asynchronous IO
        select_ret = select(remote_sock_fd + 1,&sel_fds,NULL,NULL,&timeout);

        if(select_ret == 0){
        	//printf("no socket-data, select() returns timeout \n");
        	usleep(5*1000); // 0.005s
        	continue;

        }
        else if(select_ret == -1){
        	perror("Error occurs when select()");
        	break;
        }
        if(FD_ISSET(remote_sock_fd,&sel_fds)){

        	recv_tmp = recv(remote_sock_fd, (buf+recv_sum), (RECV_BUFFER_LEN-recv_sum), 0); //1024
        	//printf("recvive from remote PC %d Bytes \n", recv_tmp);

        	if(recv_tmp > 0){
        		recv_sum += recv_tmp;
        	}
        	else if(recv_tmp == 0){
        	    net_status = RETRY;
        	    printf("Error occur when recv(), return 0 \n");
        	}
        	else if(recv_tmp == -1){
        	    net_status = RETRY;
        	    printf("Error occur when recv(), return -1 \n");
        	}

        	switch(check_buff_head(buf,recv_sum)){

        		case 1:
        	    	add_to_pack_queue(buf,recv_sum); //mutex lock
        	    	recv_sum = 0;
        	    	memset(buf,0,RECV_BUFFER_LEN);
        	    	printf("Debug:1 receive correct head and completely \n");
        	     	break;

        		case 0:
        			printf("Debug:0 receive correct head but not completely \n");
        			break;
        		case 2:
        	        heart_beat = 0;
        	        recv_sum = 0;
        	        memset(buf,0,RECV_BUFFER_LEN); //
        	        //printf("Debug:2 receive heart-beat packet \n");
        	        break;

        	    case -1:
        	    	printf("Debug:-1 receive unknown packet head -1 \n");
        	    	break;
        	    case -2:
        	    	recv_sum = 0;
        	    	memset(buf,0,RECV_BUFFER_LEN); //
        	    	printf("Debug:-2 receive unknown packet head -2 \n");
        	    	break;


        	}//check_buff_head
        }
    }

    free(buf);
    printf("sock_recv_thread exit \n");
    return((void*)0);
}

int send_data(int socket_fd, BYTE* pdata, int size)
{
	if((socket_fd == 0)||(socket_fd == -1)){
		printf("Error occurs send_data(), unexpected socket_fd \n");
		return -1;
	}
	if(pdata == NULL){
		return -1;
	}

	int ret = 0,sum = 0;
	while(sum != size){
		ret = send(socket_fd, pdata+sum, size-sum, 0);
		sum += ret;
	}
	return 0; //success
}

//sending_msg_buff(pbuff,0x41,NULL,MAX_ENV_PARAM_LEN,0x03);
int sending_msg_buff(char* pbuff,BYTE cmd,struct sockaddr_in* paddr,int length,BYTE index)
{
	if((pbuff == NULL) || (length < 0))return -1;

	//cmd
	memcpy(pbuff,&cmd,1);

	//source address
	long src_address;
	if(paddr == NULL){
		src_address = paddr->sin_addr.s_addr;//ntohl(local_sock_addr.);
	}else{
		src_address = 0;
	}
	memcpy(pbuff+1,&src_address,4);

	//msg length
	int src_length;
	src_length = htonl(length);
	memcpy(pbuff+5,&src_length,4);

	//index
	memcpy(pbuff+9,&src_length,4);

	return 0;
}

int upload_env_data()
{
	//sending_msg_buff
	return 0;
}

int upload_sample_data()
{

    return 0;
}

void* sock_listen_thread(void* arg)
{
    int addr_len = sizeof(struct sockaddr_in);

    while(!process_exit){

        if(heart_beat > 200){
            //closesocket
            net_status = RETRY;
            printf("heart beat packet timeout \n");
        }

        switch(net_status){

            case CONNECTED:
                sleep(2); //do nothing
                //process_heart_beat();
                break;

            case DISCONNECTED:
                //print_connect_info("LISTEN INFO",&local_sock_addr);
                print_local_info(&local_sock_addr);
                remote_sock_fd = accept(local_sock_fd,(struct sockaddr*)&remote_sock_addr,(socklen_t*)&addr_len);

                if(remote_sock_fd < 0){
                    perror("ERROR accept()");
                }else{
                    // accept a client successfully
                    print_connect_info(&remote_sock_addr);
                    net_status = CONNECTED;

                    if(pthread_create(&sock_recv_tid,NULL,sock_recv_thread,NULL) != 0){
                        printf("Error pthread_create(sock_listen_tid) \n");
                        exit(1);
                       }
                    /*
                    if(pthread_create(&sock_send_tid,NULL,sock_send_thread,NULL) != 0){
                        printf("Error pthread_create(sock_send_thread) \n");
                        exit(1);
                      }*/
                }
                break;

            case RETRY:
                //resolve _sock_fd and _sock_addr
                //pthread_cancel(sock_recv_tid);
                if(pthread_join(sock_recv_tid,NULL) != 0)
                    printf("Error pthread_join(sock_recv_tid) \n");
                /*
                if(pthread_join(sock_send_tid,NULL) != 0)
                		printf("Error pthread_join(sock_send_tid) \n"); */

                printf("net_status has been changed from RETRY to DISCONNECTED \n");
                net_status = DISCONNECTED;
                sleep(1);
                break;
        }
    }

    return ((void*)0); //exit successfully
}

int clean_sock(void)
{
    if(close(local_sock_fd) == -1){
        perror("ERROR close(local_sock_fd)");
        return -1;
    }
    if(close(remote_sock_fd) == -1){
        perror("ERROR close(remote_sock_fd)");
        return -1;
    }

    local_sock_fd = 0;
    remote_sock_fd = 0;
    bzero(&local_sock_addr, sizeof(struct sockaddr_in));
    bzero(&remote_sock_addr,sizeof(struct sockaddr_in));

    return 0;
}

/*print_connect_info("CONNECT INFO",&remote_sock_addr); */
void print_connect_info(struct sockaddr_in* addr)
{
    //if(addr->sin_addr == INADDR_ANY)
    printf("CONNECT INFO : %s:%u \n",inet_ntoa(addr->sin_addr),ntohs(addr->sin_port));

}

void print_local_info(const struct sockaddr_in* addr)
{
    unsigned short port = ntohs(addr->sin_port);

    register int intrface, retn = 0,fd;
	struct ifreq buf[MAXINTERFACES];
	//struct arpreq arp;
	struct ifconf ifc;
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		perror("get_ip:socket failure\n");
		return ;
	}
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;

	if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc))
	{
		intrface = ifc.ifc_len / sizeof (struct ifreq);
		while (intrface-- > 0)
		{
			//printf("interface num is intrface=%d\n\n\n",intrface);
			if(intrface<=0)
			{
				perror("get_ip:not net device\n");
				return;
			}
			if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface]))) {
				if (buf[intrface].ifr_flags & IFF_PROMISC) {
					puts ("the interface is PROMISC");
					retn++;
				}
			} else {
				char str[256];
				sprintf (str, "get_ip: ioctl device %s failure", buf[intrface].ifr_name);
				perror (str);
			}

			/*Jugde whether the net card status is up*/
			if (buf[intrface].ifr_flags & IFF_UP) {
				puts("the interface status is UP");
			}
			else {
				puts("the interface status is DOWN");
			}

			/*Get IP of the net card */
			if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
			{
				//puts ("IP address is:");
				char ipaddr[14]={'\0'};
				strcpy(ipaddr,inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
				//int iplen=strlen(ipaddr)+1;
				//strncpy(pbuff,ipaddr,*nlen>=iplen?iplen: *nlen);
				//puts (ipaddr);
				printf("listen @ %s : %u \n",ipaddr, port);

				close(fd);
				return;
			}
			else
			{
				char str[256];
				sprintf (str, "get_ip: ioctl device %s", buf[intrface].ifr_name);
				perror (str);
				return;
			}
		}
		close(fd);
	}
}
