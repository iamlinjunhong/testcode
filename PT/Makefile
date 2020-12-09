TAB = 	
CC = $(TAB)gcc
CXX = $(TAB)g++
DEBUG = -g -o2
CFLAGS = $(DEBUG) -Wall -c -fPIC
RM = rm -rf
C11 = -std=c++11
INCLUDE =-Dunixcli 

HEADER_PATH = -I./src -I./thirdpart/jsoncpp-0.10.5/include -I./thirdpart/log4cplus-1.2.1/include -I./thirdpart/gflags-2.2.1/include
LIB_PATH =  

LIBS = -lcurl -lodbc -llog4cplus -lpthread -ljson_linux-gcc-4.8.5_libmt -lgflags -lpick
TARGET = pickTrash

$(TARGET):
	cd ./src && make
	$(CXX) ./src/pickTrash.cpp -o $@ $(LIBS) $(HEADER_PATH) 
	mv $(TARGET) ./bin
	mkdir PT && cd PT && mkdir logs
	cp -rf ./bin ./PT
	cp -rf ./logConfig ./PT
	cp -rf ./lib ./PT
	cp -f env.sh ./PT
	tar -czvf PT.tar.gz PT/
	rm -rf PT/

clean:
	$(RM) ../bin/$(TARGET)
	$(RM) ../bin/std.log
	cd ./src && make clean
	$(RM) PT.tar.gz
