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
    SQLINTEGER nNativeError;
    SQLSMALLINT nErrorMsg;
    char szData[501] = {0};

    if (hstmt)
    {
        while (SQLError(henv,
                        hdbc,
                        hstmt,
                        (SQLTCHAR *)szSqlState,
                        &nNativeError,
                        (SQLTCHAR *)szError,
                        sizeof(szError) / sizeof(TCHAR),
                        &nErrorMsg) == SQL_SUCCESS)
        {
            tcharToChar(szError, szData);
            PT_ERROR("%s\n", szData);
            memset(szError, 0, sizeof(szError));
            memset(szSqlState, 0, sizeof(szSqlState));
        }
    }

    if (hdbc)
    {
        while (SQLError(henv,
                        hdbc,
                        0,
                        (SQLTCHAR *)szSqlState,
                        &nNativeError,
                        (SQLTCHAR *)szError,
                        sizeof(szError) / sizeof(TCHAR),
                        &nErrorMsg) == SQL_SUCCESS)
        {
            tcharToChar(szError, szData);
            PT_ERROR("%s\n", szData);
            memset(szError, 0, sizeof(szError));
            memset(szSqlState, 0, sizeof(szSqlState));
        }
    }

    if (henv)
    {
        while (SQLError(henv,
                        0,
                        0,
                        (SQLTCHAR *)szSqlState,
                        &nNativeError,
                        (SQLTCHAR *)szError,
                        sizeof(szError) / sizeof(TCHAR),
                        &nErrorMsg) == SQL_SUCCESS)
        {
            tcharToChar(szError, szData);
            PT_ERROR("%s\n", szData);
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

    printf("[%s:%d]%s\n", __FILE__, __LINE__, szQuery);
    returnCode = SQLExecDirect(hstmt, (SQLTCHAR *)szQuery, SQL_NTS);
    printf("[%s:%d]%s\n", __FILE__, __LINE__, szQuery);
    if ((returnCode != SQL_SUCCESS) && (returnCode != SQL_SUCCESS_WITH_INFO))
    {
        PT_ERROR("Execute \"info session\" fail\n");
        return -1;
    }
    else
    {
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
            "END"};
        int sessionId;

        returnCode = SQLFetch(hstmt);
        returnCode = SQLNumResultCols(hstmt, &num);
        if (num > 15)
        {
            num = 15;
        }
        sessionId = 0;
        while (returnCode != SQL_NO_DATA)
        {
            for (i = 1; i <= num; ++i)
            {
                returnCode = SQLGetData(hstmt, i, SQL_C_TCHAR, (SQLPOINTER)szQuery, sizeof(szQuery), &len);
                if (SQL_SUCCEEDED(returnCode))
                {
                    if (strcmp(InfoSession[sessionId], "END") != 0)
                    {
#ifdef UNICODE
                        ucToAscii((SQLWCHAR *)szQuery, szBuf);
#else
                        sprintf(szBuf, szQuery);
#endif
                        PT_ERROR("%s:%s\n", InfoSession[sessionId], szBuf);
                        sessionId++;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
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

int connect(UserInfo *pUserInfo, SQLHANDLE &henv, SQLHANDLE &hdbc)
{
    RETCODE returncode;
    henv = SQL_NULL_HANDLE;
    hdbc = SQL_NULL_HANDLE;

    TCHAR outString[1024] = {0x0};
    TCHAR connectStr[1024] = {0x0};

    SQLSMALLINT olen = 0;

    returncode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)) {
        return -1;
    }

    returncode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)) {
        /* Cleanup. No need to check return codes since we are already failing */
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }

    returncode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)) {
        /* Cleanup. No need to check return codes since we are already failing */
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }

    sprintf(connectStr, "DSN=%s;UID=%s;PWD=%s;", pUserInfo->dsn, pUserInfo->user, pUserInfo->pwd);
    returncode = SQLDriverConnect(hdbc, NULL, (SQLTCHAR *)connectStr, SQL_NTS, (SQLTCHAR *)outString, sizeof(outString), &olen, SQL_DRIVER_NOPROMPT);

    if ((returncode != SQL_SUCCESS) && (returncode != SQL_SUCCESS_WITH_INFO)) {
        PT_ERROR("[%s:%d]Call SQLConnect fail.\n", __FILE__, __LINE__);
        sqlErrors(henv, hdbc, SQL_NULL_HANDLE);

        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        return -1;
    }
    /* Connection established */
    return 0;
}

int disconnect(SQLHANDLE henv, SQLHDBC hdbc, SQLHANDLE hstmt) {
    RETCODE retcode;
    int ret = 0;

    if (hdbc) {
        retcode = SQLDisconnect(hdbc);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
            ret = -1;
            sqlErrors(henv, hdbc, hstmt);
        }
        retcode = SQLFreeConnect(hdbc);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
            ret = -1;
            sqlErrors(henv, hdbc, hstmt);
        }
    }
    if (henv) {
        retcode = SQLFreeEnv(henv);
        if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
            ret = -1;
            sqlErrors(henv, hdbc, hstmt);
        }
    }
    return ret;
}
