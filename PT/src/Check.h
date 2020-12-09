#include <string>
#include <ctype.h>

enum SQLATTRQUERY_TYPE
{
    SQL_STMT_GET_STATISTICS = 20
};

int getSqlType(std::string sql);
