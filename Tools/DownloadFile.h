#pragma once
#include "CommonTools.h"
#define CURL_STATICLIB
#include "curl/curl.h"
namespace Tools
{
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct *mem = (struct MemoryStruct *)userp;

        mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
        if (mem->memory == NULL) {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }
    static bool DownloadLink(std::wstring &link, MemoryStruct& chunk)
    {
        CURL *curl_handle;
        CURLcode res;

        chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
        chunk.size = 0;    /* no data at this point */

        curl_global_init(CURL_GLOBAL_ALL);
        /* init the curl session */
        curl_handle = curl_easy_init();
        /* specify URL to get */
        curl_easy_setopt(curl_handle, CURLOPT_URL, Tools::WStringToString(link).c_str());
        /* send all data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &Tools::WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        /* some servers don't like requests that are made without a user-agent
        field, so we provide one */
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* get it! */
        res = curl_easy_perform(curl_handle);
        /* check for errors */
        if (res != CURLE_OK) {
            //MessageBox(Tools::StringToWString(curl_easy_strerror(res)).c_str());
        }
        else {
            /*
            * Now, our chunk.memory points to a memory block that is chunk.size
            * bytes big and contains the remote file.
            *
            * Do something nice with it!
            */

            //printf("%lu bytes retrieved\n", (long)chunk.size);
        }
        long http_code = 0;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        //free(chunk.memory);

        /* we're done with libcurl, so clean it up */
        curl_global_cleanup();
        return (res == CURLE_OK && http_code == 200);
    }
}