TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c
RM = rm -rf
C11 = -std=c++11
INCLUDE = -D_WRAP_H__

SRCS = $(wildcard ./*.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

HEADER_PATH = -I.
LIB_PATH = -L/usr/lib

LIBS = -lpthread
TARGET = server 

$(TARGET):$(OBJS)
	$(CC) obj/*.o -o $@ $(LIB_PATH) $(LIBS)

$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o obj/$@ $(HEADER_PATH)

clean:
	$(RM) $(TARGET) obj/*.o
