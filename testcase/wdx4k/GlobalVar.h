#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <pthread.h>

typedef unsigned char  BYTE;
typedef unsigned short UNIT16;
typedef unsigned int   UNIT32;

struct XInstrument;
extern struct XInstrument instrument;

extern volatile unsigned char process_exit;
extern pthread_t sock_listen_tid;
extern pthread_t sock_recv_tid;
extern pthread_t sock_send_tid;


extern pthread_mutex_t queue_mutex;
extern pthread_mutex_t queue_outgoing_mutex;
extern pthread_cond_t  queue_outgoing_cond;

extern long heart_beat;
extern enum NET_STATUS net_status;//sock.c

enum XStatus{
    Idle,SwitchToIdle,
    Measuring,SwitchToMeasure,
    StandBy,SwitchToStandBy,
    ChangeSample,
    Pause,
    ContinueSampling,
    Stop,
};

extern enum XStatus instrument_status;

/* new data structure */
union FloatByte
{
    unsigned char byte[4];
    float real;
};

union IntByte
{
    unsigned char byte[4];
    int real;
};

union DoubleByte
{
    unsigned char byte[8];
    int real;
};

enum BOOLEAN {
    FALSE,
    TRUE,
};

extern void printf_errno(const char* func_name,int error);
extern void print_ids(const char* name);

#endif

