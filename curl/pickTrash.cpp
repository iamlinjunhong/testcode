#include "CommonTools.h"
#include "ODBCTools.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

#define QID_LEN 79

std::string getUrl();
std::string getQid(const std::string data);
int getTotalQidCount();

int main() {
        
    std::string URL = getUrl();
    char *postData = (char *)malloc(512*sizeof(char));
    int qidCount = getTotalQidCount();
    int currentIndex = qidCount;

    UserInfo mUserInfo;
    memset(&mUserInfo, 0, sizeof(UserInfo));
    strcpy (mUserInfo.dsn, "traf");
    strcpy (mUserInfo.user, "db__root");
    strcpy (mUserInfo.pwd, "traf123");

    while(true) {
        if(currentIndex < qidCount) {
    
            sprintf(postData, "{\"query\": {\"match_all\": {} }, \"_source\": [\"qid\"], \"from\" : %d, \"size\" : 1}", currentIndex);
            std::string result;
            CURLcode nRes = CommonTools::HttpPost(URL, std::string(postData), result, 300);
            std::string qid = getQid(result);
            std::string statistics;

            getStatistics(&mUserInfo, qid, statistics);
            std::cout << statistics << std::endl;
            currentIndex++;
            continue;
        }
        qidCount = getTotalQidCount();
    }

    return 0;
}

std::string getUrl() {
    return "http://192.168.100.194:9200/mxosrvrs_1_2020-12-03/_doc/_search?pretty";
}

std::string getQid(const std::string data) {
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["hits"]["hits"];
    std::string DevStr = DevJson[0]["_source"]["qid"].toStyledString();

    return DevStr.substr(1, DevStr.length() - 3);;
}

int getTotalQidCount() {
    std::string URL = "http://192.168.100.194:9200/mxosrvrs_1_2020-12-03/_count";
    std::string data;
    CURLcode nRes = CommonTools::HttpGet(URL, data, 300);
    Json::Reader Reader;
    Json::Value DevsJson;

    Reader.parse(data.c_str(), DevsJson);
    Json::Value DevJson = DevsJson["count"];
    std::string DevStr = DevJson.toStyledString();

    return atoi(DevStr.c_str());
}
