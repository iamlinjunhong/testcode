TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c -fPIC
RM = rm -rf
C11 = -std=c++11
INCLUDE =-Dunixcli 

HEADER_PATH = -I. -I../thirdpart/jsoncpp-0.10.5/include -I../thirdpart/log4cplus-1.2.1/include -I../thirdpart/gflags-2.2.1/include
LIB_PATH =  

LIBS = -lcurl -lodbc -llog4cplus -lpthread -ljson_linux-gcc-4.8.5_libmt -lgflags
LIB_TARGET = libpick.so

COMMON_OBJS = obj/ODBCTools.o\
			  obj/CommonTools.o\
			  obj/CMsgQueue.o\
			  obj/PlusLogger.o \
			  obj/Pick.o \
			  obj/Check.o

$(LIB_TARGET):$(COMMON_OBJS)
	$(CXX) $(COMMON_OBJS) -o $@ -shared -fPIC $(LIB_PATH) $(LIBS)
	mv $(LIB_TARGET) ../lib

obj/ODBCTools.o:ODBCTools.cpp
	$(CXX) $(CFLAGS) ODBCTools.cpp -o obj/ODBCTools.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)
	cp -rf ../thirdpart/jsoncpp-0.10.5/libs/linux-gcc-4.8.5/* ../lib/
	cp -rf ../thirdpart/log4cplus-1.2.1/lib/* ../lib/
	cp -rf ../thirdpart/gflags-2.2.1/lib/* ../lib/

obj/CommonTools.o:CommonTools.cpp
	$(CXX) $(CFLAGS) CommonTools.cpp -o obj/CommonTools.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/CMsgQueue.o:CMsgQueue.cpp
	$(CXX) $(CFLAGS) CMsgQueue.cpp -o obj/CMsgQueue.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/PlusLogger.o:PlusLogger.cpp
	$(CXX) $(CFLAGS) PlusLogger.cpp -o obj/PlusLogger.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/Pick.o:Pick.cpp
	$(CXX) $(CFLAGS) Pick.cpp -o obj/Pick.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

obj/Check.o:Check.cpp
	$(CXX) $(CFLAGS) Check.cpp -o obj/Check.o $(HEADER_PATH) $(INCLUDE) $(LIB_PATH) $(LIBS)

clean:
	$(RM) ../lib/* obj/*.o
