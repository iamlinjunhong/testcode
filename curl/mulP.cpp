#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>
#include <iomanip> //std::setprecision
#include "PlusLogger.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

int myNid = 110;
int myPid = 120;


void log4cplusInitialize(int myNid, int myPid)
{
    char logNameSuffix[32];
    const char *lv_configFileName = "log4cplus.trafodion.sql.properties";
    bool singleSqlLogFile = true;
    if (getenv("TRAF_MULTIPLE_SQL_LOG_FILE"))
        singleSqlLogFile = false;
    if (singleSqlLogFile) {
        lv_configFileName = "log4cplus.trafodion.sql.properties";
        PlusLogger::instance().initLog4cplus(lv_configFileName);
    }
    else 
    {
        sprintf( logNameSuffix, "_%d_%d.log", myNid, myPid );
        lv_configFileName = "log4cplus.trafodion.masterexe.properties";
        PlusLogger::instance().initLog4cplus(lv_configFileName, logNameSuffix);
    }
}

int main()
{
    log4cplusInitialize(myNid, myPid);
    printf("PlusLogger::isCategoryInDebug() = %d\n", PlusLogger::isCategoryInDebug("MXOSRVR"));
    pid_t pid;
    cout<<"parent have!"<<endl;
    for(int i = 0;i < 5;i++)
    {
        pid = fork();
        if(pid == 0)
        {
            break;
        }
    }
    if(pid == -1)//错误创建
    {
        cout<<"fork error"<<endl;
        _exit(1);
    }
    else if(pid == 0)//子进程
    {
        sleep(1);
        char msgBuffer[10000] = {0};
        cout<<"i am child,pid = "<<getpid()<<" my parent is:"<<getppid()<<endl;
        for(int i = 0; i < 5; i++)
        {
            sleep(1);
            sprintf(msgBuffer, "i am child,pid = %d, my parent is: %d, log count: %d", getpid(), getppid(), i);
            PlusLogger::log("MXOSRVR", PLUS_ERROR, msgBuffer);
        }
    }
    else//父进程
    {
        sleep(1);
        cout<<"i am parent,pid = "<<getpid()<<" my parent is:"<<getppid()<<endl;
    }
    cout<<"both have!"<<endl;
    return 0;
}
