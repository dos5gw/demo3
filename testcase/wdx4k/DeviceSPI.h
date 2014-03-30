#ifndef DEVICESPI_H
#define DEVICESPI_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define SPI_MAX_PACKET_LEN 64

void init_spi();

int open_spi();
int close_spi();

int spi_write(unsigned char* pbuff,unsigned int size);
int spi_read(unsigned char* pbuff,unsigned int size);

#endif
