#include "CommonTools.h"
#include "gflags/gflags.h"
#include "PlusLogger.h"
#include "Pick.h"
#include "Check.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sys/prctl.h>
#include <sstream>

#define QID_LEN 79

DEFINE_int32(lo, 1, "Listen port of this peer");
DEFINE_int32(pCount, 1, "Listen port of this peer");
DEFINE_string(ip, "http://192.168.100.194:9200", "elasticsearch ip");
DEFINE_string(date, "2020-12-03", "date");
DEFINE_string(ODBC_source, "traf", "odbc source");
DEFINE_string(userName, "trafodion", "user name");
DEFINE_string(password, "traf123", "user password");

Pick pick;
std::string getQid(const std::string data);
std::string getSql(const std::string data);
int getTotalQidCount(std::string URL);
bool getSettingResult(const std::string data);
void log4cplusInitialize();
static void copy_argv_envir(int argc,char* argv[]);
void rename_proc_title(int argc,char* argv[],const char* title);

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    log4cplusInitialize();
    PT_INFO("Initialize log4cplus successfully!");
    prctl(PR_SET_NAME, "PT", NULL, NULL, NULL);

    std::stringstream node;
    node << "mxosrvrs_" << FLAGS_lo << "_" << FLAGS_date;
    PT_INFO("Mapping %s", node.str().c_str());

    std::string searchURL(FLAGS_ip + "/" + node.str() + "/_doc/_search");
    std::string countURL(FLAGS_ip + "/" + node.str() + "/_count");

    char *postData = (char *)malloc(512*sizeof(char));
    int qidCount = getTotalQidCount(countURL);
    if(qidCount == -1) {
        PT_ERROR("Get total qid count fail, exit");
        exit(0);
    }
    int currentIndex = qidCount;
    PT_INFO("The current nums of qid is %d", qidCount);

    UserInfo mUserInfo;
    strcpy (mUserInfo.dsn, FLAGS_ODBC_source.c_str());
    strcpy (mUserInfo.user, FLAGS_userName.c_str());
    strcpy (mUserInfo.pwd, FLAGS_password.c_str());
    pick.setUserInfo(mUserInfo);
    pick.setPCount(FLAGS_pCount);
    PT_INFO("That would be %d ODBC client to connect", FLAGS_pCount);

    //start threads to fetch qid from queue, then execute sql
    pick.PickStart();
    if(!pick.isConnected()) {
        PT_ERROR("ODBC connect fail, exit");
        exit(0);
    }
    PT_INFO("Start pick trash successfully");

    while(true) {
        if(currentIndex < qidCount) {
    
            sprintf(postData, "{\"query\": {\"match_all\": {} }, \"_source\": [\"qid\", \"sqlstring\"], \"from\" : %d, \"size\" : 1}", currentIndex);
            std::string result;
            CommonTools::HttpPost(searchURL, std::string(postData), result, 300);

            if(getSqlType(getSql(result)) != SQL_STMT_GET_STATISTICS) {
                std::string qid = getQid(result);
                if(qid.empty()) continue;
                std::cout << qid << std::endl;
                pick.CollectQid(qid);
            }
            currentIndex++;
            continue;
        }
        qidCount = getTotalQidCount(countURL);
    }

    return 0;
}

std::string getQid(const std::string data) {
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["hits"]["hits"];
    std::string DevStr = DevJson[0]["_source"]["qid"].toStyledString();
    if(DevStr.find("null") != DevStr.npos) return "";

    return DevStr.substr(1, DevStr.length() - 3);
}

std::string getSql(const std::string data) {
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["hits"]["hits"];
    std::string DevStr = DevJson[0]["_source"]["sqlstring"].toStyledString();
    if(DevStr.find("null") != DevStr.npos) return "";

    return DevStr;
}

bool getSettingResult(const std::string data) {
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    std::string DevStr = DevsJson["acknowledged"].toStyledString();

    std::cout << DevStr << std::endl;
    return DevStr == "true" ? true : false;
}


int getTotalQidCount(std::string URL) {
    std::string data;
    CURLcode nRes = CommonTools::HttpGet(URL, data, 300);
    if(nRes != CURLE_OK) return -1;
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["count"];
    std::string DevStr = DevJson.toStyledString();
    if(DevStr.find("null") != DevStr.npos) return -1;

    return atoi(DevStr.c_str());
}

void log4cplusInitialize()
{
    char configFile[128] = "log4cplus.pt.properties";
    PlusLogger::instance().initLog4cplus(configFile);
}

static void copy_argv_envir(int argc,char* argv[]) {
    extern char **environ;
    int i,size=0;
    char* last=argv[argc-1]+strlen(argv[argc-1])+1;
    memset(argv[0],0,last-argv[0]+1);
    for(i=0;environ[i];i++){
        //fprintf(stderr,"%s\n",environ[i]);
        size+=strlen(environ[i])+1;
    }
    char* p=(char*)malloc(size);
    char* q=p;
    for(i=0;environ[i];i++){
        strcpy(q,environ[i]);
        environ[i]=q;
        q+=strlen(environ[i])+1;
    }
}
 
/*rename process*/
void rename_proc_title(int argc,char* argv[],const char* title) {
    copy_argv_envir(argc,argv);
    strcpy(argv[0],title);
}
