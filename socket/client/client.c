#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

#include "wrap.h"

#define SERV_IP "10.13,30.143"
#define SERV_PORT 6666


int main()
{
    int cfd;
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len;
    char buf[BUFSIZ];
    int n;

    cfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    //bind()  隐式绑定

    bzero(&serv_addr, sizeof(serv_addr));    //方式初始化失败，然后取随机值
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    Connect(cfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while(1){
        fgets(buf, sizeof(buf), stdin);   //hello world ->fgets->hello world\n\0
        send(cfd, buf, strlen(buf), 0);
        n = read(cfd, buf, sizeof(buf));
        Write(STDOUT_FILENO, buf, n);
    }
    Close(cfd);
    return 0;
}
