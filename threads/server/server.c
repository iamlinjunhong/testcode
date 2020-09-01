#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "wrap.h"

#define SERV_IP = "10.13.30.143"
#define SERV_PORT 6666
#define MAXLINE 256
struct s_info
{
    struct sockaddr_in cliaddr;
    int connfd;
};

void *do_work(void *arg);

int main()
{
    struct sockaddr_in seraddr, cliaddr;
    int listenfd, connfd;
    int clieaddr_len;
    pthread_t tid;
    struct s_info ts[256];
    int i = 0;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(SERV_PORT);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (struct sockaddr*)&seraddr, sizeof(seraddr));

    Listen(listenfd, 128);

    while(1)
    {
        clieaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr*)&cliaddr, &clieaddr_len);
        ts[i].cliaddr = cliaddr;
        ts[i].connfd = connfd;

        pthread_create(&tid, NULL, do_work, (void*)&ts[i]);
        pthread_detach(tid);
        i++;
    }

    return 0;
}

void *do_work(void *arg)
{
    int n,i;
    struct s_info* ts = (struct s_info *)arg;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];    //INET_ADDRSTRLEN用[+d查看
    while(1)
    {
        n = Read(ts->connfd, buf, MAXLINE);
        if(n == 0)
        {
            printf("the client %s closed...\n", ts->connfd);
            break;
        }
        printf("%s %d\n",
                inet_ntop(AF_INET,  &(*ts).cliaddr.sin_addr.s_addr, str, sizeof(str)),
                ntohs((*ts).cliaddr.sin_port));
        
        for(i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        Write(STDOUT_FILENO, buf, n);
        Write(ts->connfd, buf, n);
    }
    Close(ts->connfd);

    return (void *)0;
}

















