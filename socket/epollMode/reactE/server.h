#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include "wrap.h"

#define MAX_EVENTS 1024
#define SERV_PORT 6666
#define SERV_IP "10.13.30.143"
#define BUFLEN 4096

struct myevent_s {
    int fd;                                           //要监听的文件描述符
    int events;                                       //对应的监听事件
    void *arg;                                        //泛型参数
    void (*call_back)(int fd, int events, void *arg); //回调函数
    int status;                                       //1->在红黑树上，0->不在红黑树上
    char buf[BUFLEN];                                 //数据
    int len;                                            
    long last_active;                                 //记录每次加入红黑树 g_efd的时间
};


//接受数据
void recvdata(int fd, int events, void *arg);

//发送数据
void senddata(int fd, int events, void *arg);

//创建socket，初始化lfd
void initlistensocket(int efd, short port);

//将结构体myevents_s成员变量初始化
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg, char data[]);

//向epoll监听的红黑树添加一个文件描述符
void eventadd(int efd, int events, struct myevent_s *ev);

//从epoll监听的红黑树中删除一个文件描述符
void eventdel(int efd, struct myevent_s *ev);

//当有文件描述符就绪，epoll返回，调用该函数于客户端建立连接
void acceptconn(int lfd, int events, void *arg);

bool log4cplusInitialize();

#endif
