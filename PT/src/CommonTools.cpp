#include "CommonTools.h"


size_t CommonTools::receive_data(void *contents, size_t size, size_t nmemb, void *stream){
    std::string *str = (std::string*)stream;
    (*str).append((char*)contents, size*nmemb);
    return size * nmemb;
}

size_t CommonTools::writedata2file(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int CommonTools::download_file(const char* url, const char outfilename[FILENAME_MAX]){
    CURL *curl;
    FILE *fp;
    CURLcode res;
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != res)
    {
        printf("init libcurl failed.");
        curl_global_cleanup();
        return -1;
    }
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename,"wb");

        res = curl_easy_setopt(curl, CURLOPT_URL, url);
        if (res != CURLE_OK)
        {
            fclose(fp);
            curl_easy_cleanup(curl);
            return -1;
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CommonTools::writedata2file);
        if (res != CURLE_OK){
            fclose(fp);
            curl_easy_cleanup(curl);
            return -1;
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        if (res != CURLE_OK)
        {
            fclose(fp);
            curl_easy_cleanup(curl);
            return -1;
        }

        res = curl_easy_perform(curl);
        fclose(fp);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return -1;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}

CURLcode CommonTools::HttpGet(const std::string & strUrl, std::string & strResponse,int nTimeout){
    CURLcode res;
    CURL* pCURL = curl_easy_init();

    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);
    curl_easy_setopt(pCURL, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, CommonTools::receive_data);
    curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);
    res = curl_easy_perform(pCURL);
    curl_easy_cleanup(pCURL);
    return res;
}

CURLcode CommonTools::HttpPost(const std::string & strUrl, std::string szJson, std::string & strResponse, int nTimeout) {
    CURLcode res;
    char szJsonData[1024];
    memset(szJsonData, 0, sizeof(szJsonData));
    strcpy(szJsonData, szJson.c_str());
    CURL* pCURL = curl_easy_init();
    struct curl_slist* headers = NULL;
    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }
    
    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());

    curl_easy_setopt(pCURL, CURLOPT_POST, 1L);

    headers = curl_slist_append(headers,"content-type:application/json");

    curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, szJsonData);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);
    curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, CommonTools::receive_data);
    curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);

    res = curl_easy_perform(pCURL);

    curl_easy_cleanup(pCURL);
    curl_slist_free_all(headers);

    return res;
}

CURLcode CommonTools::HttpPut(const std::string & strUrl, std::string szJson, std::string & strResponse, int nTimeout) {
    CURLcode res;
    char szJsonData[1024];
    memset(szJsonData, 0, sizeof(szJsonData));
    strcpy(szJsonData, szJson.c_str());
    CURL* pCURL = curl_easy_init();
    struct curl_slist* headers = NULL;
    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }
    
    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());

    curl_easy_setopt(pCURL, CURLOPT_PUT, 1L);

    headers = curl_slist_append(headers,"content-type:application/json");

    curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, szJsonData);
    curl_easy_setopt(pCURL, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);
    curl_easy_setopt(pCURL, CURLOPT_READFUNCTION, CommonTools::receive_data);
    curl_easy_setopt(pCURL, CURLOPT_READDATA, (void*)&strResponse);
    curl_easy_setopt(pCURL, CURLOPT_INFILESIZE_LARGE, 1024);

    res = curl_easy_perform(pCURL);

    curl_easy_cleanup(pCURL);
    curl_slist_free_all(headers);

    return res;
}
