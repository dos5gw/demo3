CC=gcc
OBJ=main.o Sock.o DeviceI2C.o DeviceSPI.o Packet.o rs232.o Xrfunit.o
LIB=-lpthread
CFLAG=-g -Wall
xrf:$(OBJ)
	@ $(CC) $(CFLAG) -o $@ $^ $(LIB)
main.o:main.c GlobalVar.h Sock.h rs232.h DeviceI2C.h DeviceSPI.h
	$(CC) $(CFLAG) -c $<
Sock.o:Sock.c GlobalVar.h Sock.h
	$(CC) $(CFLAG) -c $<
DeviceI2C.o:DeviceI2C.c GlobalVar.h DeviceI2C.h
	$(CC) $(CFLAG) -c $<
DeviceSPI.o:DeviceSPI.c GlobalVar.h DeviceSPI.h
	$(CC) $(CFLAG) -c $<
Packet.o:Packet.c GlobalVar.h Packet.h
	$(CC) $(CFLAG) -c $<
rs232.o:rs232.c rs232.h
	$(CC) $(CFLAG) -c $<
xrfunit.o:xrfunit.c GlobalVar.h
	$(CC) $(CFLAG) -c $<
clean:
	-rm *.o xrf
