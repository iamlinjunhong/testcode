#include "server.h"

int g_efd;
struct myevent_s g_events[MAX_EVENTS+1];

int main(int argc, char *argv[])
{
    unsigned short port = SERV_PORT;
    
    if(argc == 2){
        port = atoi(argv[1]);
    }

    g_efd = epoll_create(MAX_EVENTS+1);
    if(g_efd <=0 ){
        printf("create efd in %s err %s\n", __func__, strerror(errno));
    }

    initlistensocket(g_efd, port);

    struct epoll_event events[MAX_EVENTS+1];
    printf("server running:port[%d]\n", port);

    int checkpos = 0, i;
    while(1){
  /*      long now = time(NULL);  //当前时间
        for(i = 0; i < 100; i++, checkpos++){
            if(checkpos == MAX_EVENTS){
                checkpos = 0;
            }
            if(g_events[checkpos].status != 1){
                continue;
            }
            long duration = now - g_events[checkpos].last_active;

            if(duration >= 60){
                close(g_events[checkpos].fd);
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);
            }
        }*/

        int nfd = epoll_wait(g_efd, events, MAX_EVENTS+1, 1000);
        if(nfd < 0){
            printf("epoll_wait error, exit\n");
            break;
        }

        for(i = 0; i < nfd; i++){
            struct myevent_s *ev = (struct myevent_s*)events[i].data.ptr;
            
            if((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)){
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
            if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)){
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
        }
    }

    return 0;
}

//接受数据
void recvdata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)malloc(sizeof(struct myevent_s));
    ev = arg;
    int len;

    len = recv(fd, ev->buf, sizeof(ev->buf), 0);

    eventdel(g_efd, ev);

    if(len > 0){
        ev->len = len;
        ev->buf[len] = '\0';
        printf("receive data from [%d], data:%s\n", fd, ev->buf);
        
        eventset(ev, fd, senddata, ev, ev->buf);
        eventadd(g_efd, EPOLLOUT, ev);
    }else if(len == 0){
        close(ev->fd);
        printf("[fd=%d] , closed\n", fd);
    }else {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return ;
}

//发送数据
void senddata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;

    len = send(fd, ev->buf, ev->len, 0);

    if(len > 0){
        printf("send[fd=%d], [%d]%s\n", fd, len, ev->buf);
        eventdel(g_efd, ev);
        eventset(ev, fd, recvdata, ev, NULL);
        eventadd(g_efd, EPOLLIN, ev);
    }else {
        close(ev->fd);
        eventdel(g_efd, ev);
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
    }


    return ;
}

//创建socket，初始化lfd
void initlistensocket(int efd, short port)
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS], NULL);

    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(lfd, (struct sockaddr*)&sin, sizeof(sin));
    
    listen(lfd, 20);
    return ;
}

//将结构体myevents_s成员变量初始化
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg, char data[])
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    if(data != NULL)
    {    
        ev->len = strlen(data);
        strcpy(ev->buf, data);
    }else {
        memset(ev->buf, 0, sizeof(ev->buf));
        ev->len = 0;
    }
    ev->last_active = time(NULL);

    return ;
}

//向epoll监听的红黑树添加一个文件描述符
void eventadd(int efd, int events, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;
    if(ev->status == 1){
        op = EPOLL_CTL_MOD;
    }else {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if(epoll_ctl(efd, op, ev->fd, &epv) < 0){
        printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);   
    }else{
        printf("event add OK [fd=%d], op=%d \n", ev->fd, op);
    }
    return ;
} 

//从epoll监听的红黑树中删除一个文件描述符
void eventdel(int efd, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    if(ev->status != 1){
        return ;
    }
    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);

    return ;
}

//当有文件描述符就绪，epoll返回，调用该函数于客户端建立连接
void acceptconn(int lfd, int events, void *arg)
{
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd, i;

    if((cfd = accept(lfd, (struct sockaddr *)&cin, &len)) == -1){
        if(errno != EAGAIN && errno != EINTR){
            printf("errno != EAGAIN && errno != EINTR\n");
        }       
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return ;
    }

    do{
        for(i = 0; i < MAX_EVENTS; i++){
            if(g_events[i].status == 0){
                break;
            }
        }
        if(i == MAX_EVENTS) {
            printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
            break;
        }

        eventset(&g_events[i], cfd, recvdata, &g_events[i], NULL);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
    }while(0);

    printf("new connect [%s:%d] [time:%ld], pos[%d]\n",
            inet_ntoa(cin.sin_addr), 
            ntohs(cin.sin_port), 
            g_events[i].last_active, 
            i);
}

