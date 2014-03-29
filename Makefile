# Template: Dropbox/Fedora One/Makefile
include make.mk
DIR=$(shell pwd)
DIR_BIN = $(DIR)
DIR_INCLUDE = $(DIR)/include
DIR_SOURCE = $(DIR)/src
DIR_CPP_SOURCE = $(DIR)/cpp
DIR_OBJ = $(DIR)/obj

#SRCEXTS = .c .cpp .c++
#HDREXTS = .h. .hpp .h++
# Move this to make.mk

APP = demo2
CPP_EXTENSION=cpp

CPP_SRCS = $(foreach d,$(DIR_CPP_SOURCE),$(wildcard $(d)/*.cpp) )
CPP_OBJS = $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(notdir $(CPP_SRCS)))
SRCS = $(foreach d,$(DIR_SOURCE),$(wildcard $(d)/*.c) )
OBJS = $(patsubst %.c,$(DIR_OBJ)/%.o,$(notdir $(SRCS)))


vpath %.cpp $(dir $(CPP_SRCS))
vpath %.c $(dir $(SRCS))
vpath %.o $(OBJS) 

CC = gcc
CXX = g++
CXXFLGAS = -Wall -W -g
CCFLAGS = -Wall -W -g
INCLUDE+= -I. -I$(DIR_INCLUDE)
LDFLAGS += -lpthread -lstdc++

all: $(APP)

$(CPP_OBJS): $(DIR_OBJ)/%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

$(OBJS): $(DIR_OBJ)/%.o: %.c
	$(CXX) -c $(CFLAGS) -o $@ $<

$(APP): $(OBJS) $(CPP_OBJS)
	$(CXX) $^ -o $(APP) $(LDFLAGS) $(INCLUDE)

clean: 
	$(RM) -fr $(DIR_OBJ)/*.o
	$(RM) -fr $(DIR)/$(APP)
