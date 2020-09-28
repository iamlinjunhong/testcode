#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "wrap.h"

#define SERV_PORT 6666
#define SERV_IP "10.13.30.143"

#define MAXLINE 8192
#define OPEN_MAX 5000

#define LOG(s) printf("%s\n", s);

int main()
{
    int listenfd, connfd, sockfd;
    int efd;
    int res;
    int nready;
    int i, j, n;
    socklen_t clie_addr_len;
    char buf[MAXLINE], str[INET_ADDRSTRLEN];
    struct sockaddr_in serv_addr, clie_addr;
    struct epoll_event tep, ep[OPEN_MAX];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    LOG("Socke()")

    //端口复用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    LOG("setportopt()")

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    LOG("Bind()")

    Listen(listenfd, 128);
    LOG("Listen()")
    
    //创建epoll模型，efd指向红黑树根节点
    efd = epoll_create(OPEN_MAX);
    LOG("epoll_create()")
    if(efd == -1){
        perr_exit("epoll_create error");
    }

    tep.events = EPOLLIN;
    //设置listenfd监听事件为读
    tep.data.fd = listenfd;
    //将listenfd及对应的结构体设置到树上，efd可找到该树
    res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);
    LOG("epoll_ctl()")
    if(res == -1){
        perr_exit("epoll_ctl error");
    }

    while(1) {
        //epoll为server阻塞监听事件，ep为struct epoll_event类型数组，OPEN_MAX为数组容量，-1表示永久阻塞
        nready = epoll_wait(efd, ep, OPEN_MAX, -1);
        LOG("epoll_wait()")
        if(nready == -1){
            perr_exit("epoll_wait error");
        }
        for(i = 0; i < nready; i++){
            if(!(ep[i].events & EPOLLIN)){
                continue;
            }
            if(ep[i].data.fd == listenfd){
                clie_addr_len = sizeof(clie_addr);
                connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
                LOG("Accept()")

                printf("receive from %s at PORT %d\n",
                        inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str)),
                        ntohs(clie_addr.sin_port));

                tep.events = EPOLLIN;
                tep.data.fd = connfd;
                res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
                if(res == -1) {
                    perr_exit("epoll_ctl error");
                }
            }else {
                sockfd = ep[i].data.fd;
                //这里的读事件不会发生阻塞
                n = Read(sockfd, buf, MAXLINE);
                
                if(n == 0){
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    if(res == -1) {
                        perr_exit("epoll_ctl error");
                    }
                    Close(sockfd);
                    printf("client[%d] closed connection\n", sockfd);
                }else if(n < 0){
                    perror("readn < 0, error:");
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    Close(sockfd);
                }else {
                    for(j = 0; j < n; j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    //写事件可能发生阻塞
                    Write(STDOUT_FILENO, buf, n);
                    Writen(sockfd, buf, n);
                }

            }
        }
    }
    
    Close(listenfd);
    Close(efd);
    return 0;
}
