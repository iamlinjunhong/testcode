#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>
#include "wrap.h"

#define SERV_PORT 6666
#define SERV_IP 10.13.30.143
#define LOG(s) printf("%s\n",s);

int main(int argc, char *argv[])
{
    int listenfd, connfd, sockfd, maxfd;
    //自定义数组client，防止遍历allset，FD_SETSIZE默认为1024
    int client[FD_SETSIZE];
    int i, j, n, maxi, nready, opt;
    //#define INET_ADDRSTRLEN 16
    char buf[BUFSIZ], str[INET_ADDRSTRLEN];
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    //rset 读事件文件描述符集合，allset用来存储所有的fd
    fd_set rset, allset;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    LOG("socket()")

    opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    LOG("setsockopt()")

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    LOG("bind()")

    Listen(listenfd, 1024);
    LOG("listen()")
    
    //初始listenfd即为最大的文件描述符
    maxfd = listenfd;

    //maxi指向client数组的最后位置
    maxi = -1;
    for(i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }

    //构造select监控文件描述符集
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1)
    {
        //每次循环都新设置select监控信号集
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        LOG("select()")
        if(nready < 0)
        {
            perr_exit("select error");
        }
        //满足条件，则说明有新的客户端的请求
        if(FD_ISSET(listenfd, &rset))
        {
            clie_addr_len = sizeof(clie_addr);
            //accept此时并不会阻塞
            connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
            LOG("accept()")
            printf("receive from %s at PORT %d\n",
                    inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str)),
                    ntohs(clie_addr.sin_port));
                for(i = 0; i < FD_SETSIZE; i++)
                {
                    //找最前client中没有使用的位置
                    if(client[i] < 0)
                    {
                        client[i] = connfd;
                        break;
                    }
                }
            if(i == FD_SETSIZE)
            {
                fputs("too many clients\n", stderr);
                exit(1);
            }
            
            FD_SET(connfd, &allset);
            LOG("FD_SET(connfd, &allset)")
            //select第一次参数需要
            if(connfd > maxfd)
            {
                maxfd = connfd;
            }
            if(i > maxi)
            {
                maxi = i;
            }
            if(--nready == 0)
            {
            continue; 
            }
        }
        for(i = 0; i <= maxi; i++)
        {
            if((sockfd = client[i]) < 0)
                continue;
            //遍历检查哪一个client有数据就绪
            if(FD_ISSET(sockfd, &rset))
            {
                LOG("read()")
                //当client关闭连接时，服务器也关闭连接
                if((n = Read(sockfd, buf, sizeof(buf))) == 0)
                {
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                else if(n > 0)
                {
                    for(j = 0; j < n; j++)
                    {
                        buf[j] = toupper(buf[j]);
                    }
                    Write(sockfd, buf, n);
                    LOG("write()")
                }
                if(--nready == 0)
                    break;
            }
        }
    }

    Close(listenfd);
    LOG("close()")
    return 0;
}
