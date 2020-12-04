#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <fcntl.h>
#define SERV_IP "192.168.100.193"
#define SREV_PORT 6666 

int main()
{
    int lfd;
    int cfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clie_addr;
    socklen_t clie_addr_len, clie_IP_len;
    char buf[20], clie_IP[BUFSIZ];
    int n;
    int i;
    int ret;
    int t = 0;
    int len;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == lfd)
    {
        perror("socket error");
        exit(1);
    }

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));       //设置端口允许被复用

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SREV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY自动获取本机网络上的有效IP

    ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen error");
        exit(1);
    }
    printf("Listen has been executed!\n");
    clie_addr_len = sizeof(clie_addr);     //传入传出参数                                                                
    cfd = accept(lfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
    if(cfd == -1)
    {
        perror("accept error");
        exit(1);
    }

    int keepIdle = 1;
    int keepInterval = 1;
    int keepCount = 1;
    setsockopt(listenfd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
    setsockopt(listenfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
    setsockopt(listenfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

   i/* int error = 0;
    int isKeepalive = 1;
    int keepaliveIdle = 1;
    int keepaliveInterval = 1;
    int keepCount = 1;
    error += setsockopt(cfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&isKeepalive , sizeof(isKeepalive));
    error += setsockopt(cfd, SOL_SOCKET, TCP_KEEPIDLE, (void*)&keepaliveIdle , sizeof(keepaliveIdle )); 
    error += setsockopt(cfd, SOL_SOCKET, TCP_KEEPINTVL, (void *)&keepaliveInterval , sizeof(keepaliveInterval ));                                                                                              
    error += setsockopt(cfd, SOL_SOCKET, TCP_KEEPCNT, (void *)&keepCount , sizeof(keepCount )); 
    if (error != 0){ 
        printf("error----------------------\n");
    }*/
 

    inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP));
    printf("client IP:%s, client port:%d\n", clie_IP, ntohs(clie_addr.sin_port));
    len = 0;
    while(len < 20)
    {
        printf("this is %d time recv\n", t++);
        n = recv(cfd, buf, 2, 0);
        if(n < 0) break;
       /* for(i = 0; i < n; i++)
        {
           buf[i] = toupper(buf[i]);
        }*/
        len += 2;
        printf("recv: %s\n", buf);
        printf("len = %d\n", len);
    }
    close(lfd);
    close(cfd);

    return 0;
}
