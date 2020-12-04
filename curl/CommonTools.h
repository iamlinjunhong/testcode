#include <iostream>
#include "curl/curl.h"
#include "zlib.h"
#include <vector>
#include <string>
#include <unistd.h>
#include <memory.h>
#include "json/json.h"
#include <sstream>

class CommonTools{
    public:
        CommonTools();
        ~CommonTools();
    public:
        static size_t receive_data(void *contents, size_t size, size_t nmemb, void *stream);

        static size_t writedata2file(void *ptr, size_t size, size_t nmemb, FILE *stream);

        static int download_file(const char* url, const char outfilename[FILENAME_MAX]);

        static CURLcode HttpGet(const std::string & strUrl, std::string & strResponse,int nTimeout);

        static CURLcode HttpPost(const std::string & strUrl, std::string szJson,std::string & strResponse,int nTimeout);
};

