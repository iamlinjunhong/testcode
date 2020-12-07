TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c
RM = rm -rf
C11 = -std=c++11
INCLUDE =-Dunixcli 

HEADER_PATH = -I. -I/home/aubin/testcode/curl/jsoncpp-0.10.5/include -I/home/aubin/testcode/curl/log4cplus-1.2.1/include -I/home/aubin/testcode/curl/gflags-2.2.1/include
LIB_PATH = -L/usr/lib -L/home/aubin/testcode/curl/jsoncpp-0.10.5/libs/linux-gcc-4.8.5 -L/home/aubin/testcode/curl/log4cplus-1.2.1/lib -L/home/aubin/testcode/curl/gflags-2.2.1/lib

LIBS = -lcurl -lodbc -llog4cplus -lpthread -ljson_linux-gcc-4.8.5_libmt -lgflags
TARGET = pickTrash

COMMON_OBJS = obj/pickTrash.o\
			  obj/ODBCTools.o\
			  obj/CommonTools.o\
			  obj/CMsgQueue.o\
			  obj/PlusLogger.o \
			  obj/Pick.o

$(TARGET):$(COMMON_OBJS)
	$(CXX) $(COMMON_OBJS) -o $@ $(LIB_PATH) $(LIBS)

obj/pickTrash.o:pickTrash.cpp
	$(CXX) $(CFLAGS) pickTrash.cpp -o obj/pickTrash.o $(HEADER_PATH) $(INCLUDE) 

obj/ODBCTools.o:ODBCTools.cpp
	$(CXX) $(CFLAGS) ODBCTools.cpp -o obj/ODBCTools.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/CommonTools.o:CommonTools.cpp
	$(CXX) $(CFLAGS) CommonTools.cpp -o obj/CommonTools.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/CMsgQueue.o:CMsgQueue.cpp
	$(CXX) $(CFLAGS) CMsgQueue.cpp -o obj/CMsgQueue.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/PlusLogger.o:PlusLogger.cpp
	$(CXX) $(CFLAGS) PlusLogger.cpp -o obj/PlusLogger.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/Pick.o:Pick.cpp
	$(CXX) $(CFLAGS) Pick.cpp -o obj/Pick.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

clean:
	$(RM) $(TARGET) obj/*.o
