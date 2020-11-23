
#include "ZKClient.h"
 
/***********************************
*函数：                     ZKClient
*功能：                     构造函数
*参数：                     无
*返回值：                   无
*备注：                     无
***********************************/
ZKClient::ZKClient()
{
    ZK_init();
}
 
/***********************************
*函数：                     ~ZKClient
*功能：                     析构函数
*参数：                     无
*返回值：                   无
*备注：                     无
***********************************/
ZKClient::~ZKClient()
{
}
 
/***********************************
*函数：                     ZK_init
*功能：                     初始化函数
*参数：                     无
*返回值：                   无
*备注：                     无
***********************************/
void ZKClient::ZK_init()
{
    m_zkHandle = NULL;
    time_out = 100;
    version = -1;
    max_file_path = 1000;
    max_length = 512;
}
 
/***********************************
*函数：                 ZK_connect
*功能：                 连接zookeeper功能
*参数：                 port                    zookeeper的IP:PORT
*返回值：               0                           成功
                                -1                      失败
*备注：无
***********************************/
int ZKClient::ZK_connect(const char* port)
{
    m_zkHandle = zookeeper_init(port,NULL,time_out,0,(void*)this,0);
    if(m_zkHandle == NULL)
    {
        return -1;
    }
    //设置打印等级
    zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
 
    return 0;
}
 
/***********************************
*函数：             ZK_close
*功能：             关闭zookeeper功能
*参数：             无
*返回值：           0                               成功
                            -1                          失败
*备注：             无
***********************************/
int ZKClient::ZK_close()
{
    int retval;
    retval = zookeeper_close(m_zkHandle);
    if(ZOK != retval)
    {
        return -1;
    }
    return 0;
}
 
/***********************************
*函数：             ZK_set
*功能：             设置节点属性
*参数：             path                        节点位置
                            buffer                  节点属性
*返回值：           0                               成功
                            -1                          失败
                            1                               节点不存在
*备注：             无
***********************************/
int ZKClient::ZK_set(const char *path,const char *buffer)
{
 
    int len = strlen(buffer);
  int retval;
 
    if(ZK_exists(path))
        return 1;
    
    
    retval = zoo_set(m_zkHandle,path,buffer,len,version);
    if(ZOK != retval)
        return -1;
 
    return 0;
}
 
/***********************************
*函数：             ZK_get
*功能：             获取节点属性
*参数：             path                        节点位置
                            buffer                  存放节点属性
*返回值：           0                               成功
                            -1                          失败
                            1                               节点不存在
*备注：             无
***********************************/
int ZKClient::ZK_get(const char *path,char *buffer)
{
    char getbuf[max_length];
    memset(getbuf,0,sizeof(getbuf));
  int retval;
 
    if(ZK_exists(path))
        return 1;
    retval = zoo_get(m_zkHandle,path,0,getbuf,&max_length,NULL);
    if(ZOK != retval)
        return -1;
    printf("buffer=[%s]\n",getbuf);
    strcpy(buffer,getbuf);
    return 0;
}
 
/***********************************
*函数：             ZK_exists
*功能：             判断节点是否存在
*参数：             path                        节点位置
*返回值：           0                               节点存在
                            -1                          节点不存在
*备注：             无
***********************************/
int ZKClient::ZK_exists(const char *path)
{
    
    int retval;
    retval = zoo_exists(m_zkHandle,path,0,NULL);
    if(ZOK != retval)
    {
        return -1;
    }
    return 0;
}
 
/***********************************
*函数：             ZK_create
*功能：             新建节点
*参数：             path                        节点位置
                            value                       节点属性
*返回值：           0                               成功
                            -1                          失败
                            1                               节点已经存在
*备注：             无
***********************************/
int ZKClient::ZK_create(const char *path ,const char *value)
{
    int retval;
    if(0 == ZK_exists(path))
        return 1;
 
    retval = zoo_create(m_zkHandle,path,value,strlen(value),&ZOO_OPEN_ACL_UNSAFE,0,(char*)path,max_file_path);
    if(ZOK != retval)
    {
        return -1;
    }
    return 0;
}
 
/***********************************
*函数：             ZK_delete
*功能：             删除节点
*参数：             path                        节点位置
*返回值：           0                               成功
                            -1                          失败
                            1                               节点未存在
*备注：             无
***********************************/
int ZKClient::ZK_delete(const char * path)
{
    int retval;
    if(ZK_exists(path))
        return 1;
    
    retval = zoo_delete(m_zkHandle,path,version);
    if(ZOK != retval)
    {
        return -1;
    }
    return 0;
}
