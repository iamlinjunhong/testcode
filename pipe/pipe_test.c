#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{

    int fd[2];
    pid_t pid;

    int ret = pipe(fd);
    if(ret == -1){
        perror("pipe error:");
        exit(1);
    }

    pid = fork();
    if(pid == -1){
        perror("pipe error:");
        exit(1);
    }else if(pid == 0) {         //子进程，读数据
        close(fd[1]);
        char buf[1024];
        ret = read(fd[0], buf, sizeof(buf));
        if(ret == 0) {
            printf("---------------------------------\n");
        }
        write(STDOUT_FILENO, buf, ret);
    }else {
        close(fd[0]);            //父进程，写数据
        write(fd[1], "hello pipe\n", sizeof("hello pipe\n"));
    }
    return 0;
}
