#include "ODBCTools.h"

void ucToAscii(SQLWCHAR *uc, char *ascii)
{
    int i;

    for (i = 0; uc[i]; i++)
    {
        ascii[i] = uc[i] & 0x00ff;
    }

    ascii[i] = 0;
}

void charToTChar(char *szChar, TCHAR *szTChar)
{
#ifdef UNICODE
    int i;

    for (i = 0; szChar[i]; i++)
    {
        szTChar[i] = szChar[i];
    }
    szTChar[i] = 0;
#else
    sprintf(szTChar, "%s", szChar);
#endif
}
void tcharToChar(TCHAR *szTChar, char *szChar)
{
#ifdef UNICODE
    int i;

    for (i = 0; szTChar[i]; i++)
    {
        szChar[i] = szTChar[i] & 0x00ff;
    }

    szChar[i] = 0;
#else
    sprintf(szChar, "%s", szTChar);
#endif
}

int sqlErrors(SQLHANDLE henv, SQLHDBC hdbc, SQLHANDLE hstmt)
{
    TCHAR szError[501];
    TCHAR szSqlState[10];
    SQLINTEGER  nNativeError;
    SQLSMALLINT nErrorMsg;
    char szData[501] = {0};

    if ( hstmt ){
        while ( SQLError( henv, 
                        hdbc, 
                        hstmt, 
                        (SQLTCHAR *)szSqlState, 
                        &nNativeError, 
                        (SQLTCHAR *)szError, 
                        sizeof(szError) / sizeof(TCHAR),
                        &nErrorMsg ) == SQL_SUCCESS ){
            tcharToChar(szError, szData);
            LogMsg(NONE, "%s\n", szData);
            memset(szError, 0, sizeof(szError));
            memset(szSqlState, 0, sizeof(szSqlState));
        }
    }

    if ( hdbc ){
        while ( SQLError( henv, 
                            hdbc, 
                            0, 
                            (SQLTCHAR *)szSqlState, 
                            &nNativeError, 
                            (SQLTCHAR *)szError, 
                            sizeof(szError) / sizeof(TCHAR), 
                            &nErrorMsg ) == SQL_SUCCESS ){
            tcharToChar(szError, szData);
            LogMsg(NONE, "%s\n", szData);
            memset(szError, 0, sizeof(szError));
            memset(szSqlState, 0, sizeof(szSqlState));
        }
    }

    if ( henv ){
        while ( SQLError( henv, 
                            0, 
                            0, 
                            (SQLTCHAR *)szSqlState, 
                            &nNativeError, 
                            (SQLTCHAR *)szError, 
                            sizeof(szError) / sizeof(TCHAR),
                            &nErrorMsg ) == SQL_SUCCESS ){
            tcharToChar(szError, szData);
            LogMsg(NONE, "%s\n", szData);
            memset(szError, 0, sizeof(szError));
            memset(szSqlState, 0, sizeof(szSqlState));
        }
    }

    return 1;
}
int printSession(SQLHSTMT hstmt)
{
    SQLRETURN returnCode;
    char szBuf[256] = {0};
    TCHAR szQuery[256] = {0};
    SQLLEN len;
    int i;

    charToTChar("info session", szQuery);
    printf("[%s:%d]%s\n", __FILE__, __LINE__, szQuery);
    returnCode = SQLExecDirect(hstmt, (SQLTCHAR*)szQuery, SQL_NTS);
    printf("[%s:%d]%s\n", __FILE__, __LINE__, szQuery);
    if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, "Execute \"info session\" fail\n");
        return -1;
    }
    else{
        SQLSMALLINT num;
        const char *InfoSession[] = {
            "SESSION_ID",
            "SERVER_PROCESS_NAME",
            "SERVER_PROCESS_ID",
            "SERVER_HOST",
            "SERVER_PORT",
            "MAPPED_SLA",
            "MAPPED_CONNECT_PROFILE",
            "MAPPED_DISCONNECT_PROFILE",
            "CONNECTED_INTERVAL_SEC",
            "CONNECT_TIME",
            "DATABASE_USER_NAME",
            "USER_NAME",
            "ROLE_NAME",
            "APP_NAME",
            "TENANT_NAME",
            "END"
        };
        int sessionId;
        
        returnCode = SQLFetch(hstmt);
        returnCode = SQLNumResultCols(hstmt, &num);
        if(num > 15){
            num = 15;
        }
        sessionId = 0;
        while (returnCode != SQL_NO_DATA)
        {
            for (i = 1; i <= num; ++i)
            {
                returnCode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)szQuery, sizeof(szQuery), &len);
                if (SQL_SUCCEEDED(returnCode)){
                    if(strcmp(InfoSession[sessionId], "END") != 0){
#ifdef UNICODE
                        ucToAscii((SQLWCHAR *)szQuery, szBuf);
#else
                        sprintf(szBuf, szQuery);
#endif
                        LogMsg(NONE, "%s:%s\n", InfoSession[sessionId], szBuf);
                        sessionId++;
                    }
                    else{
                        break;
                    }
                }
                else{
                    break;
                }
            }
            returnCode = SQLFetch(hstmt);
        }
    }
    SQLFreeStmt(hstmt, SQL_UNBIND);
    SQLFreeStmt(hstmt, SQL_CLOSE);
        
    return 0;
}

int driverConnect(UserInfo *pUserInfo, int Options)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR szOut[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR szTURL[1024] = {0x0};
    char szURL[1024] = {0};
    
	assert( (Options&CONNECT_ODBC_VERSION_2) || (Options&CONNECT_ODBC_VERSION_3) );

	/* Initialize the basic handles needed by ODBC */
	if (Options&CONNECT_ODBC_VERSION_2)
	{
		returncode = SQLAllocEnv(&henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return -1;
        }
		
		returncode = SQLAllocConnect(henv,&hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeEnv(henv);
			return -1;
		}
	}
	if (Options&CONNECT_ODBC_VERSION_3)
	{
		returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return -1;
        }

		returncode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return -1;
		}

		returncode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return -1;
		}
	}

    sprintf(szURL, "DSN=%s;UID=%s;PWD=%s;%s", 
                    pUserInfo->dsn, 
                    pUserInfo->user, 
                    pUserInfo->pwd, 
                    pUserInfo->url);
    LogMsg(NONE, "URL:%s\n", szURL);
    charToTChar(szURL, szTURL);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)szTURL, SQL_NTS, (SQLTCHAR*)szOut, sizeof(szOut), &olen, SQL_DRIVER_NOPROMPT);
    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
	{
	    LogMsg(NONE, "[%s:%d]Call SQLConnect fail.\n", __FILE__, __LINE__);
        sqlErrors(henv, hdbc, SQL_NULL_HANDLE);
		// Free up handles since we hit a problem.
		if (Options&CONNECT_ODBC_VERSION_2){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}
		if (Options&CONNECT_ODBC_VERSION_3){
			/* Cleanup.  No need to check return codes since we are already failing */		   
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
            return -1;
		}
        return -1;
	}

	pUserInfo->henv = (SQLHANDLE)henv;
	pUserInfo->hdbc = (SQLHANDLE)hdbc;
	
	/* Connection established */
	return 0;
}

int sqlConnect(UserInfo *pUserInfo, int Options)
{
    RETCODE returncode;
    SQLHANDLE henv = SQL_NULL_HANDLE;
    SQLHANDLE hdbc = SQL_NULL_HANDLE;
    TCHAR OutString[1024] = {0x0};
    SQLSMALLINT olen = 0;
    TCHAR connectStr[1024] = {0x0};
    char szDdl[1024] = {0};
    
	assert( (Options&CONNECT_ODBC_VERSION_2) || (Options&CONNECT_ODBC_VERSION_3) );

	/* Initialize the basic handles needed by ODBC */
	if (Options&CONNECT_ODBC_VERSION_2)
	{
		returncode = SQLAllocEnv(&henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return -1;
        }
		
		returncode = SQLAllocConnect(henv,&hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeEnv(henv);
			return -1;
		}
	}
	if (Options&CONNECT_ODBC_VERSION_3)
	{
		returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
            return -1;
        }

		returncode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return -1;
		}

		returncode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			/* Cleanup. No need to check return codes since we are already failing */
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			return -1;
		}
	}

	// Handle Autocommit_Off Option
	if (Options&CONNECT_AUTOCOMMIT_OFF)
    {/*
		returncode = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
		if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)){
			// Cleanup.  No need to check return codes since we are already failing
			SQLFreeEnv(henv);
			return -1;
		}*/
	}  
#if 1
    sprintf(szDdl, "DSN=%s;UID=%s;PWD=%s;%s", pUserInfo->dsn, pUserInfo->user, pUserInfo->pwd, "");
    charToTChar(szDdl, connectStr);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR*)connectStr, SQL_NTS, (SQLTCHAR*)OutString, sizeof(OutString), &olen, SQL_DRIVER_NOPROMPT);
#else
    TCHAR dsn[128], user[128], pwd[128];

    charToTChar(pUserInfo->dsn, dsn);
    charToTChar(pUserInfo->user, user);
    charToTChar(pUserInfo->pwd, pwd);
    returncode = SQLConnect(hdbc, (SQLTCHAR *)dsn, SQL_NTS, (SQLTCHAR *)user, SQL_NTS, (SQLTCHAR *)pwd, SQL_NTS);
#endif
    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO))
	{
	    LogMsg(NONE, "[%s:%d]Call SQLConnect fail.\n", __FILE__, __LINE__);
        sqlErrors(henv, hdbc, SQL_NULL_HANDLE);
		// Free up handles since we hit a problem.
		if (Options&CONNECT_ODBC_VERSION_2){
			/* Cleanup.  No need to check return codes since we are already failing */
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
			return -1;
		}
		if (Options&CONNECT_ODBC_VERSION_3){
			/* Cleanup.  No need to check return codes since we are already failing */		   
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
            return -1;
		}
        return -1;
	}

	pUserInfo->henv = (SQLHANDLE)henv;
	pUserInfo->hdbc = (SQLHANDLE)hdbc;
	
	/* Connection established */
	return 0;
}
int disconnectDb(SQLHANDLE henv, SQLHDBC hdbc, SQLHANDLE hstmt)
{
    RETCODE retcode;                        
    int ret = 0;

    if(hdbc){
        retcode = SQLDisconnect(hdbc);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            ret = -1;	
            sqlErrors(henv, hdbc, hstmt);
        }
        retcode = SQLFreeConnect(hdbc);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            ret = -1;	
            sqlErrors(henv, hdbc, hstmt);
        }
    }
    if(henv){
        retcode = SQLFreeEnv(henv);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            ret = -1;	
            sqlErrors(henv, hdbc, hstmt);
        }
    }

    return ret;
}
int getStatistics(UserInfo *pUserInfo, const std::string qid, std::string &statistics)
{
    int ret = 0;
    RETCODE retcode;
    SQLHANDLE   henv = SQL_NULL_HANDLE;
    SQLHANDLE   hdbc = SQL_NULL_HANDLE;
    SQLHANDLE   hstmt = SQL_NULL_HANDLE;
    SQLLEN len;

    UserInfo mUserInfo;
    char szBuf[512] = {0};
    int index;
    SWORD numOfCols, col;

    memcpy(&mUserInfo, pUserInfo, sizeof(UserInfo));

    retcode = sqlConnect(&mUserInfo, CONNECT_ODBC_VERSION_3);
    if(retcode != SQL_SUCCESS) {
        LogMsg(INFO, "connect fail: line %d\n");
        sqlErrors(henv,hdbc,hstmt);
        return -1;
    }

    
    henv = mUserInfo.henv;
    hdbc = mUserInfo.hdbc;
    
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        LogMsg(NONE, "SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) fail,line = %d\n", __LINE__);
        LogMsg(NONE,"Try SQLAllocStmt((SQLHANDLE)hdbc, &hstmt)\n");
        retcode = SQLAllocStmt((SQLHANDLE)hdbc, &hstmt);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
            LogMsg(NONE,"SQLAllocStmt hstmt fail, line %d\n", __LINE__);
            disconnectDb(henv, hdbc, hstmt);
            return -1;
        }
    }
    
    sprintf(szBuf, "get statistics for qid %s;", qid.c_str());
    LogMsg(NONE, "SQLExecDirect(hstmt, \"%s\", SQL_NTS)\n", szBuf);
    retcode = SQLExecDirect(hstmt, (SQLTCHAR*)szBuf, SQL_NTS);
    if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
        printf("[%s:%d]call SQLExecDirect fail:%d\n", __FILE__, __LINE__, retcode);
        sqlErrors(henv, hdbc, hstmt);
        ret = -1;
    }
    else{
        retcode = SQLFetch(hstmt);
        numOfCols = 0;
        index = 0;
        retcode = SQLNumResultCols(hstmt, &numOfCols);
        len = 0;
        SQLRowCount(hstmt, &len);
        LogMsg(NONE, "total columns:%d  total rows:%d\n", numOfCols, len);
        while ((retcode != SQL_NO_DATA) && (numOfCols != 0)){
            for (col = 1; col <= numOfCols; col++){
                memset(szBuf, 0, sizeof(szBuf));
                retcode = SQLGetData(hstmt, col, SQL_C_TCHAR, (SQLPOINTER)szBuf, sizeof(szBuf) - 1, &len);
                if (retcode == SQL_SUCCESS){
                    LogMsg(NONE, "%s\n", szBuf);
                    statistics.append(szBuf);
                }
                else{
                    LogMsg(NONE, "not found statistics for qid %s\n", qid.c_str());
                    statistics.append("not found statistics for qid ");
                    statistics.append(qid);
                    sqlErrors(henv, hdbc, hstmt);
                    ret = -1;
                    break;
                }
            }
            retcode = SQLFetch(hstmt);
            if(retcode != SQL_SUCCESS) {
                LogMsg(NONE, "Fail to get data\n");
                sqlErrors(henv, hdbc, hstmt);
                ret = -1;
                break;
            }
        }
    }

    disconnectDb(henv, hdbc, hstmt);
    return ret;
}

