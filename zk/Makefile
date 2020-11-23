TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c
RM = rm -rf
C11 = -std=c++11
INCLUDE = -D_WRAP_H__

SRCS = $(wildcard ./*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

HEADER_PATH = -I. -I/home/aubin/dependency/tools/zookeeper-3.4.5/include
LIB_PATH = -L/usr/lib -L/home/aubin/dependency/tools/zookeeper-3.4.5/lib -L/usr/lib64

LIBS = -lpthread -lzookeeper_mt -lzookeeper_st
TARGET = demo

$(TARGET):$(OBJS)
	$(CXX) $^ -o $@ $(LIB_PATH) $(LIBS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(HEADER_PATH)

clean:
	$(RM) $(TARGET) *.o
