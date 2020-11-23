#include "ZKClient.h"
#include <unistd.h>

int main()
{
    int ret = -1;
    ZKClient *zk_client = new ZKClient() ;
    //ZooKeeper的IP:PORT
    char port[] = "10.13.30.142:2181,10.13.30.143:2181,10.13.30.144:2181";
    ret = zk_client->ZK_connect(port);
    if(ret)
    {
        printf("连接zookeeper失败\n");
        return -1;
    }
    printf("连接zookeeper成功\n");
 
 
    char path[] = "/test2";
    char value[] = "the node is create by lsf";
    ret = zk_client->ZK_create(path,value);
    if(0 > ret)
        printf("创建node失败\n");
    else if(0 == ret)
        printf("创建node成功\n");
    else
        printf("此node已经存在\n");
 
 
    char buffer[512];
    while(1) {
        memset(buffer,0,sizeof(buffer));
        ret = zk_client->ZK_get(path,buffer);
        if(0 > ret)
            printf("获取node信息失败\n");
        else if(0 == ret)
            printf("获取node信息成功buffer:[%s]\n",buffer);
        else
            printf("此node不存在\n");
        sleep(2);
    }
    char setbuf[] = "change by lsf";
    ret = zk_client->ZK_set(path,setbuf);
    if(0 > ret)
        printf("修改node信息失败\n");
    else if(0 == ret)
        printf("修改node信息成功\n");
    else
        printf("此node不存在\n");
 
 
    memset(buffer,0,sizeof(buffer));
    ret = zk_client->ZK_get(path,buffer);
    if(0 > ret)
        printf("获取node信息失败\n");
    else if(0 == ret)
        printf("获取node信息成功buffer:[%s]\n",buffer);
    else
        printf("此node不存在\n");
 
 
    ret = zk_client->ZK_delete(path);
    if(0 > ret)
        printf("删除node失败\n");
    else if(0 == ret)
        printf("删除node成功\n");
    else
        printf("此node不存在\n");
 
    ret = zk_client->ZK_close();
    if(ret)
    {
        printf("关闭zookeeper失败\n");
        return -1;
    }
    printf("关闭zookeeper成功\n");
 
    delete zk_client;
    
    return 0;
}
