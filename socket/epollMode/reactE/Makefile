TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c
RM = rm -rf
C11 = -std=c++11
INCLUDE = -D_WRAP_H_ -D_PLUSLOGGER_H _ -D_LOG_H_
export LD_LIBRARY_PATH=/usr/lib

SRCS = $(wildcard ./*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

HEADER_PATH = -I. -I./pluslog -I/usr/include/log4cplus
LIB_PATH = -L/usr/lib

LIBS = -lpthread -llog4cplus
TARGET = server 

$(TARGET):$(OBJS)
	$(CXX) obj/*.o -o $@ $(LIB_PATH) $(LIBS) 

$(OBJS):%.o:%.cpp
	$(CXX) $(CFLAGS) $< -o obj/$@ $(HEADER_PATH) 

clean:
	$(RM) $(TARGET) obj/*.o
