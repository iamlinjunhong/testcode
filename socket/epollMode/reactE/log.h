#ifndef _LOG_H_
#define _LOG_H_

#include "PlusLogger.h"

#define LOG_WRAN(s) \
    PlusLogger::log("REACT", PLUS_WARN, s);

#endif
