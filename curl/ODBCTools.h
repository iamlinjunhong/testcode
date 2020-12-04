#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef unixcli
#include <unistd.h>
#include <pthread.h>
#else
#include <windows.h>
#include <tchar.h>
#endif
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlext.h>

#ifdef unixcli
#ifndef TCHAR
#define TCHAR    char
#endif
#endif/*end unixcli*/
#include <assert.h>



#define CONNECT_AUTOCOMMIT_OFF        0x01    // Default is on.
#define CONNECT_ODBC_VERSION_2        0x02
#define CONNECT_ODBC_VERSION_3        0x04

#define NONE            0x00     /* Don't use any options.  Equivalent to using LogPrintf() */
#define ERRMSG          0x01     /* Prefix the <ErrorString> to the front of the message */
#define TIME_STAMP      0x02     /* Display a timestamp on the line before the message */
#define LINEBEFORE      0x04     /* Display separating lines before and after the message */
#define LINEAFTER       0x08     /* Display a separating line after the message */
#define ENDLINE         LINEAFTER/* Some older programs used to use ENDLINE instead of LINEAFTER */
#define INTERNALERRMSG  0x10     /* Prefix ***INTERNAL ERROR: to the fromt of the message */
#define SHORTTIMESTAMP  0x20     /* Prefix a shorter timestamp to the front of the message */
#define INFO                    0x30     /* Will print only if debugMode = on */
#define LINE_NUM        0x100

#define LogMsg(Options,format,...) do{if(Options) printf("[%s:%d] ", __FILE__, __LINE__);printf(format,##__VA_ARGS__);}while(0)

int sqlErrors(SQLHANDLE henv, SQLHDBC hdbc, SQLHANDLE hstmt);

#define ODBC_ERROR(henv, hdbc, hstmt) do{LogMsg(NONE, "[%s:%d]", __FILE__, __LINE__);\
    sqlErrors(henv, hdbc, hstmt); }while (0)


typedef  struct tabUserInfo
{
    SQLHANDLE henv;
    SQLHDBC hdbc;
    SQLHANDLE hstmt;
    char dsn[128];
    char user[128];
    char pwd[50];
    char url[128];
}UserInfo;

void ucToAscii(SQLWCHAR *uc, char *ascii);
void charToTChar(char *szChar, TCHAR *szTChar);
void tcharToChar(TCHAR *szTChar, char *szChar);
int sqlErrors(SQLHANDLE henv, SQLHDBC hdbc, SQLHANDLE hstmt);
int printSession(SQLHSTMT hstmt);
int driverConnect(UserInfo *pUserInfo, int Options);
int sqlConnect(UserInfo *pUserInfo, int Options);
int disconnectDb(SQLHANDLE henv, SQLHDBC hdbc, SQLHANDLE hstmt);
int getStatistics(UserInfo *pUserInfo, const std::string qid, std::string &statistics);
