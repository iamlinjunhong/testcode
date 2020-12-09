#ifndef Pick_DEFINED
#define Pick_DEFINED
#include <string>
#include "CMsgQueue.h"
#include "ODBCTools.h"
#include "PlusLogger.h"

class Pick {
private:
	UserInfo mUserInfo;
	CMsgQueue q;
	pthread_t tids[10];
    int pCount;
    bool connected;

public:
    Pick():pCount(1), connected(false) {};
    
    ~Pick() {};

    bool isConnected();

    void setPCount(int pCount);

	//configure user info
	void setUserInfo(UserInfo mUserInfo);

	//collect qid
	void CollectQid(std::string qid);

	//start multiple thread
	void PickStart();

	//consume qid
	static void *ExecuteQid(void *arg);
};
#endif
