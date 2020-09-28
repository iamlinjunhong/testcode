#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#include <wrap.h>

#define LOGMSG(s) printf("%s\n", s);
#define SERV_ADDR "serv.socket"

int main()
{
    int lfd, cfd, len, size, i;
    struct sockaddr_un servaddr, cliaddr;
    char buf[4096];

    lfd = Socket(AF_UNIX, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SERV_ADDR);

    len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path);

    unlink(SERV_ADDR);         //确保bind之前serv.sock, bind会创建该文件

    Bind(lfd, (struct sockaddr *)&servaddr, len);

    Listen(lfd, 20);
    
    while(1){
        len = sizeof(cliaddr);
        cfd = Accept(lfd, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
        LOGMSG("Accept()")

        len -= offsetof(struct sockaddr_un, sun_path);
        cliaddr.sun_path[len] = '\0';

        printf("client bind filename %s\n", cliaddr.sun_path);

        while((size = read(cfd, buf, sizeof(buf))) > 0){
            for(i = 0; i < size; i++)
                buf[i] = toupper(buf[i]);
            Write(cfd, buf, size);
        }
        close(cfd);
    }
    close(lfd);

    return 0;
}