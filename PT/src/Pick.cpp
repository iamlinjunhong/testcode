#include "Pick.h"

void Pick::setUserInfo(UserInfo mUserInfo) {
	this->mUserInfo = mUserInfo;
}

void Pick::setPCount(int pCount) {
	this->pCount = pCount;
}

void Pick::CollectQid(std::string qid){
	q.Push(qid);
}

void Pick::PickStart() {
	for(int i = 0;i < pCount; i++) {
		int ret = pthread_create(&tids[i], NULL, ExecuteQid, (void*) this);
        if(ret != 0) 
            PT_ERROR("Create pthread to pick fail");
        usleep(100000);
	}
}

bool Pick::isConnected() {
    return this->connected;
}

void *Pick::ExecuteQid(void * arg) {
	Pick * _this = (Pick*) arg;
  
    RETCODE retcode;

    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    SQLLEN len;
    SWORD numOfCols, col;

    char szBuf[1024] = {0};

    retcode = connect(&_this->mUserInfo, henv, hdbc);
    if(retcode != SQL_SUCCESS) {
        sqlErrors(henv,hdbc,hstmt);
        goto _FAIL;
    }
    _this->connected = true;
    PT_INFO("ODBC Connect successfully");
    

	while(true) {
		if(_this->q.Ready() && _this->q.Size() > 0) {
            string qid = _this->q.Pop();
            string statistics = "";

            retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
                if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                    PT_ERROR("SQLAllocStmt hstmt fail, line %d\n", __LINE__);
                    continue;
                }
            }

            sprintf(szBuf, "get statistics for qid %s", qid.c_str());
            retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
                sqlErrors(henv, hdbc, hstmt);
                continue;
            } 
            retcode = SQLFetch(hstmt);
            if(retcode != SQL_SUCCESS) {
                sqlErrors(henv, hdbc, hstmt);
                continue;
            } 
            numOfCols = 0;
            retcode = SQLNumResultCols(hstmt, &numOfCols);
            len = 0;
            SQLRowCount(hstmt, &len);

            while ((retcode != SQL_NO_DATA) && (numOfCols != 0)){
                for (col = 1; col <= numOfCols; col++){
                    memset(szBuf, 0, sizeof(szBuf));
                    retcode = SQLGetData(hstmt, col, SQL_C_TCHAR, (SQLPOINTER)szBuf, sizeof(szBuf) - 1, &len);
                    if (retcode == SQL_SUCCESS){
                        statistics.append(szBuf);
                        statistics.append("\n");
                    } else {
                        break;
                    }
                }
                retcode = SQLFetch(hstmt);
                if(retcode != SQL_SUCCESS) {
                    //sqlErrors(henv, hdbc, hstmt);
                    break;
                }
            }   
            if(statistics != "") {
                PT_INFO("\n%s", statistics.c_str());
            }
        }
    }

_FAIL:
    disconnect(henv, hdbc, hstmt);
    PT_INFO("Exit from thread");
    pthread_exit(NULL);
}


