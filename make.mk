# compiler define
SHELL	= /bin/bash
COMPILE_DIR = $(shell pwd)

#ARCH ?= mipsel-linux
# compile and bin utilies
#CC		= $(TOOLCHAIN_DIR)/bin/$(ARCH)-gcc  
#AR		= $(TOOLCHAIN_DIR)/bin/$(ARCH)-ar   
#LD		= $(TOOLCHAIN_DIR)/bin/$(ARCH)-ld   
#RUN		= $(TOOLCHAIN_DIR)/bin/$(ARCH)-run  
#DB		= $(TOOLCHAIN_DIR)/bin/$(ARCH)-gdb  
#LINK    = $(TOOLCHAIN_DIR)/bin/$(ARCH)-gcc  
#CPP     = $(TOOLCHAIN_DIR)/bin/$(ARCH)-g++
#CXX     = $(TOOLCHAIN_DIR)/bin/$(ARCH)-g++
#RANLIB  = $(TOOLCHAIN_DIR)/bin/$(ARCH)-ranlib
#AS      = $(TOOLCHAIN_DIR)/bin/$(ARCH)-as
#STRIP	= $(TOOLCHAIN_DIR)/bin/$(ARCH)-strip
#NM		= $(TOOLCHAIN_DIR)/bin/$(ARCH)-nm

# shell commands
CP 		= cp
MV		= mv

# generic flags 
#DEFS = -D_POSIX_SOURCE
#FLAGS  = -EL -c -mips32 -O2 -fno-builtin -W ${DEFS} 
#FLAGS  = -EL -g -c -mips32 -O2 -fno-builtin -W ${DEFS}
