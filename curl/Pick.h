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
	pthread_t tids[5];

public:
    Pick();
    
    ~Pick();

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
