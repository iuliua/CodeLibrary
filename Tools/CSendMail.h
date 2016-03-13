#define CURL_STATICLIB
#include "curl/curl.h"
#include <string>
#include <sstream>
#ifdef _DEBUG
#pragma comment(lib,"libcurld-ssl.lib")
#else
#pragma comment(lib,"libcurl-ssl.lib")
#endif
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"ssleay32.lib")
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"wldap32.lib")

class CSendMail
{
    struct upload_status {
        char* buffer;
        int len;
        int read;
    };
    static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
    {
        struct upload_status *upload_ctx = (struct upload_status *)userp;

        if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
            return 0;
        }
        if (upload_ctx->read == upload_ctx->len)
            return 0;
        else
        {
            int len = upload_ctx->len - upload_ctx->read;
            if (len > 64)
                len = 64;
            memcpy(ptr, upload_ctx->buffer + upload_ctx->read, len);
            upload_ctx->read += len;
            return len;
        }
    }
public:
    static CURLcode SendMail(const char* server, BOOL secure, const char* user, const char *passwd, const char* from, const char* sender_name, const char* to, const char * subject, const char* body)
    {
        CURLcode res = CURLE_OK;
        struct curl_slist *recipients = NULL;
        struct upload_status upload_ctx = { 0 };
        CURL *curl = curl_easy_init();
        if (!curl)
            return CURLE_FAILED_INIT;
        curl_easy_setopt(curl, CURLOPT_USERNAME, user);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd);
        std::string server_url;
        if (secure)
            server_url = "smtps://";
        else
            server_url = "smtp://";
        server_url += server;
        curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);
        recipients = curl_slist_append(recipients, to);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        std::ostringstream oss;
        oss << "To: " << to << std::endl;
        oss << "From: " << from << "(" << sender_name << ")" << std::endl;
        oss << "Subject: " << subject << std::endl;
        oss << std::endl;
        oss << body << std::endl;
        char buffer[5 * 1024];
        strcpy_s(buffer, _countof(buffer), oss.str().c_str());
        upload_ctx.buffer = buffer;
        upload_ctx.len = oss.str().size() + 1;
        upload_ctx.read = 0;

        res = curl_easy_perform(curl);

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        return res;
    }
};