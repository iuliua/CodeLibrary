#include "curl/curl.h"
#include <string>
#include <sstream>

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
    static CURLcode SendMail(const char* server, const char* user, const char *passwd, const char* from, const char* sender_name, const char* to,const char* bcc, const char * subject, const char* body)
    {
        CURLcode res = CURLE_OK;
        struct curl_slist *recipients = NULL;
        struct upload_status upload_ctx = { 0 };
        CURL *curl = curl_easy_init();
        if (!curl)
            return CURLE_FAILED_INIT;
        curl_easy_setopt(curl, CURLOPT_USERNAME, user);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd);
        std::string server_url = "smtp://";
        server_url += server;
        curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);
        recipients = curl_slist_append(recipients, to);
        if (strlen(bcc) > 0)
            recipients = curl_slist_append(recipients, bcc);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        char buffer[5*1024];
        sprintf_s(buffer, _countof(buffer),
            "To: <%s> \r\n"
            "From: <%s>(%s) \r\n"
            "Subject: %s \r\n"
            "\r\n"
            "%s", to, from, sender_name, subject, body);
        upload_ctx.buffer = buffer;
        upload_ctx.len = strlen(buffer);
        upload_ctx.read = 0;

        res = curl_easy_perform(curl);

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        return res;
    }
};