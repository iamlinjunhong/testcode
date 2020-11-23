#ifndef _ZKCLIENT_INCLUDE_
#define _ZKCLIENT_INCLUDE_

#include <iostream>
#include "zookeeper/zookeeper.h"
#include <assert.h>
#include <string.h>
using namespace std;

class ZKClient
{
    private:
        zhandle_t *m_zkHandle;      //zookeeper操作句柄
        int time_out;                           //会话过期时间
        int version;                            //版本号
        int max_file_path;              //path_buffer的长度
        int max_length;                     //读取数据长度
    public:
        ZKClient();
        ~ZKClient();
        void ZK_init();                                                                     //初始化函数
        int ZK_connect(const char *port);                                   //连结zookeeper函数
        int ZK_close();                                                                     //关闭zookeeper函数
        int ZK_get(const char *path,char *buffer);              //获取node信息
        int ZK_set(const char *path,const char *buffer);    //设置节点信息
        int ZK_exists(const char *path);                                    //判断节点是否存在
        int ZK_create(const char *path,const char* value);//创建一个新节点
        int ZK_delete(const char *path);                                    //删除一个节点
};

#endif
