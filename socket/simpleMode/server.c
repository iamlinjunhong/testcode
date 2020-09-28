#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <string.h>

#define SERV_IP "10.13.30.143"
#define SREV_PORT 6666 

int main()
{
    int lfd;
    int cfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clie_addr;
    socklen_t clie_addr_len, clie_IP_len;
    char buf[BUFSIZ], clie_IP[BUFSIZ];
    int n;
    int i;
    int ret;

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


    inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP));
    printf("client IP:%s, client port:%d\n", clie_IP, ntohs(clie_addr.sin_port));
    while(1)
    {
        n = read(cfd, buf, sizeof(buf));
        printf("Before: %s", buf);
        for(i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        printf("After: %s", buf);
        write(cfd, buf, n);
    }
    close(lfd);
    close(cfd);

    return 0;
}
