#ifndef DEVICEIIC_H
#define DEVICEIIC_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define IIC_MAX_PACKET_LEN 64

void init_iic();

int open_iic();
int close_iic();

int iic_write(unsigned char* pbuff,unsigned int size);
int iic_read(unsigned char* pbuff,unsigned int size);

#endif

