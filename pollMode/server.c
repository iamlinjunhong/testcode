#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netinet/in.h>
#include <errno.h>

#include "wrap.h"

#define SERV_IP "10.13.30.143"
#define SERV_PORT 6666
#define MAXLINE 80
#define OPEN_MAX 1024
#define LOG(s) printf("%s\n",s);

int main()
{
    int i, j, n;
    int maxi, nready;
    socklen_t clie_len;
    int listenfd, connfd, sockfd;
    char buf[BUFSIZ], str[INET_ADDRSTRLEN];
    struct sockaddr_in serv_addr, clie_addr;
    struct pollfd client[OPEN_MAX];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    LOG("Socket()")

    //端口复用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    LOG("setsockopt")

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    LOG("Bind()")

    Listen(listenfd, 128);
    LOG("Listen()")

    client[0].fd = listenfd;
    client[0].events = POLLIN;
    
    for(i = 1; i <= OPEN_MAX; i++){
        client[i].fd = -1;   
    }

    maxi = 0;

    while(1){
        nready = poll(client, maxi+1, -1);
        LOG("poll()")
        //此时有新的client发起连接
        if(client[0].revents & POLLIN){
            clie_len = sizeof(clie_addr);
            connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_len);
            LOG("Accept()")
            printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str)),
                    ntohs(clie_addr.sin_port));

            for(i = 0; i < OPEN_MAX; i++){
                if(client[i].fd < 0){
                    client[i].fd = connfd;
                    break;
                }
            }
            
            if(i == OPEN_MAX){
                perr_exit("too many client");
            }
            client[i].events = POLLIN;
            if(i > maxi){
                maxi = i;
            }
            if(--nready == 0)
                continue;
        }
        
        for(i = 1; i <= maxi; i++){
            if((sockfd = client[i].fd) < 0)
                continue;
            if(client[i].revents & POLLIN){
                LOG("Read()")
                if((n = Read(sockfd, buf, MAXLINE)) < 0){
                    //收到RST标志
                    if(errno == ECONNRESET){
                        printf("client[%d] aborted connection\n", i);
                        Close(sockfd);
                        client[i].fd = -1;
                    }else
                        perr_exit("read error");
                }else if(n == 0){
                    printf("client[%d] closed connection\n", i);
                    Close(sockfd);
                    client[i].fd = -1;
                }else{
                    for(j = 0; j < n; j++){
                        buf[j] = toupper(buf[j]);
                    }
                    Write(sockfd, buf, n);
                    LOG("Write()")
                }
                if(--nready <= 0)
                    break;
            }
        }
    }

    return 0;
}
