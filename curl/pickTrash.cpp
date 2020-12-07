#include "CommonTools.h"
#include "gflags/gflags.h"
#include "PlusLogger.h"
#include "Pick.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

#define QID_LEN 79

DEFINE_int32(port, 9200, "Listen port of this peer");
DEFINE_string(ip, "http://192.168.100.194:9200", "elasticsearch ip");
DEFINE_string(node, "mxosrvrs_1_2020-12-03", "mxosrvr node and date");
DEFINE_string(ODBC_source, "traf", "odbc source");
DEFINE_string(userName, "db__root", "user name");
DEFINE_string(password, "traf123", "user password");

Pick pick;
std::string getQid(const std::string data);
int getTotalQidCount(std::string URL);
int getSqlType(const std::string data);
void log4cplusInitialize();

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    
    std::string searchURL(FLAGS_ip + "/" + FLAGS_node + "/_doc/_search");
    std::string countURL(FLAGS_ip + "/" + FLAGS_node + "/_count");
    std::cout << FLAGS_node << std::endl;

    char *postData = (char *)malloc(512*sizeof(char));
    int qidCount = getTotalQidCount(countURL);
    PT_INFO("qid count = %d", qidCount);
    int currentIndex = qidCount;

    UserInfo mUserInfo;
    strcpy (mUserInfo.dsn, FLAGS_ODBC_source.c_str());
    strcpy (mUserInfo.user, FLAGS_userName.c_str());
    strcpy (mUserInfo.pwd, FLAGS_password.c_str());
    pick.setUserInfo(mUserInfo);

    log4cplusInitialize();
    PT_INFO("Initialize log4cplus successfully!");

    pick.PickStart();
    PT_INFO("Start pick trash successfully");

    while(true) {
        if(currentIndex < qidCount) {
    
            sprintf(postData, "{\"query\": {\"match_all\": {} }, \"_source\": [\"qid\"], \"from\" : %d, \"size\" : 1}", currentIndex);
            std::string result;
            CURLcode nRes = CommonTools::HttpPost(searchURL, std::string(postData), result, 300);

            std::string qid = getQid(result);
            std::cout << qid << std::endl;
            pick.CollectQid(qid);
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

    return DevStr.substr(1, DevStr.length() - 3);
}

int getSqlType(const std::string data) {
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["hits"]["hits"];
    std::string DevStr = DevJson[0]["_source"]["type"].toStyledString();

    return atoi(DevStr.c_str());
}



int getTotalQidCount(std::string URL) {
    std::string data;
    CURLcode nRes = CommonTools::HttpGet(URL, data, 300);
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["count"];
    std::string DevStr = DevJson.toStyledString();

    return atoi(DevStr.c_str());
}

void log4cplusInitialize()
{
    char configFile[64] = "log4cplus.pt.properties";
    PlusLogger::instance().initLog4cplus(configFile);
}
