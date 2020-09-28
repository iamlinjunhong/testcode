#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#include "wrap.h"

#define SERV_ADDR "/home/aubin/workspace/testcode/socket/localMode/serv.socket"
#define CLIE_ADDR "clie.socket"
#define LOGMSG(s) printf("%s\n", s);

int main()
{
    int cfd, len;
    struct sockaddr_un servaddr, cliaddr;
    char buf[4096];

    cfd = Socket(AF_UNIX, SOCK_STREAM, 0);

    //client
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sun_family = AF_UNIX;
    strcpy(cliaddr.sun_path, CLIE_ADDR);

    len = offsetof(struct sockaddr_un, sun_path) + strlen(cliaddr.sun_path);

    unlink(CLIE_ADDR);
    Bind(cfd, (struct sockaddr *)&cliaddr, len);
    
    //server
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SERV_ADDR);

    len = offsetof(struct sockaddr_un, sun_path) + strlen(servaddr.sun_path);

    Connect(cfd, (struct sockaddr*)&servaddr, len);
    LOGMSG("Connect()")

    while(fgets(buf, sizeof(buf), stdin) != NULL){
        Write(cfd, buf, strlen(buf));
        LOGMSG("Write()")

        len = Read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
    }
    close(cfd);

    return 0;
}
