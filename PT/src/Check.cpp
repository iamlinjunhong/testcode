#include "Check.h"

int getSqlType(std::string sql) {
    for (size_t j = 0; j < sql.length(); j++)
        sql[j] = toupper(sql[j]); 

    if(sql.find("STATISTICS") != sql.npos) {
        return SQL_STMT_GET_STATISTICS;
    }
    return -1;
}
