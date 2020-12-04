TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c
RM = rm -rf
C11 = -std=c++11
INCLUDE = 

HEADER_PATH = -I../log4cplus-1.2.1/include
LIB_PATH = -L/usr/lib

LIBS = -llog4cplus -lpthread
TARGET = cplus

$(TARGET):PlusLogger.o mulP.o
	$(CXX) PlusLogger.o mulP.o -o cplus $(LIB_PATH) $(LIBS)

mulP.o:mulP.cpp
	$(CXX) $(CFLAGS) mulP.cpp -o mulP.o $(HEADER_PATH)

PlusLogger.o:PlusLogger.cpp
	$(CXX) $(CFLAGS) PlusLogger.cpp -o PlusLogger.o $(HEADER_PATH)

clean:
	$(RM) $(TARGET) *.o
