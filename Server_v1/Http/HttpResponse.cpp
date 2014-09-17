#include "HttpResponse.h"

#include <string>
#include <sys/stat.h>


map<int, const char*> __InitCodeToMsg()
{
    map<int, const char*> dict;
    dict.insert(pair<int, const char*>(200, "OK"));
    dict.insert(pair<int, const char*>(404, "Not Found"));
    dict.insert(pair<int, const char*>(405, "Method Not Allowed"));
    return dict;
}


map<string, const char*> __InitExtensionToContentType()
{
    map<string, const char*> dict;
    dict.insert(pair<string, const char*>("html", "text/html"));
    dict.insert(pair<string, const char*>("css", "text/css"));
    dict.insert(pair<string, const char*>("js", "text/javascript"));
    dict.insert(pair<string, const char*>("jpg", "image/jpeg"));
    dict.insert(pair<string, const char*>("jpeg", "image/jpeg"));
    dict.insert(pair<string, const char*>("png", "image/png"));
    dict.insert(pair<string, const char*>("gif", "image/gif"));
    dict.insert(pair<string, const char*>("swf", "application/x-shockwave-flash"));
    return dict;
}


HttpResponse::HttpResponse()
{
    _code = 0;
}


void HttpResponse::SetCode(int code)
{
    _code = code;
}


int HttpResponse::GetCode()
{
    return _code;
}


bool HttpResponse::IsCodeDefined()
{
    return _code != 0;
}

const char* HttpResponse::GetCodeMsg()
{
    static const map<int, const char*> codeToMsg = __InitCodeToMsg();

    auto iter = codeToMsg.find(_code);
    if (iter != codeToMsg.end())
    {
        return iter->second;
    }
    return "";
}


void HttpResponse::SetFilePath(const string &path, const string &extension)
{
    _filePath = path;
    _extension = extension;
}

#include <iostream>
#include <string.h>

void HttpResponse::WriteToBuffer(evbuffer *buf)
{
    time_t rawTime = time(NULL);
    tm *timeptr = gmtime(&rawTime);

    static const char wday_name[][4] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char mon_name[][4] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static const map<string, const char*> extensionToContentType = __InitExtensionToContentType();

    evbuffer_add_printf(buf,

                        "HTTP/%.1f %d %s\r\n"
                        "Date: %.3s,%3d %.3s %d %.2d:%.2d:%.2d GMT\r\n"
                        "Server: Ksan.Server.v0.01\r\n"
                        "Connection: close\r\n",

                        GetVersion(), GetCode(), GetCodeMsg(),
                        wday_name[timeptr->tm_wday], timeptr->tm_mday, mon_name[timeptr->tm_mon], 1900 + timeptr->tm_year, timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec

    );

    if (GetCode() == 200)
    {
        string contentTypeString = "text/plain";
        auto contentTypePair = extensionToContentType.find(_extension);
        if (contentTypePair != extensionToContentType.end())
        {
            contentTypeString = contentTypePair->second;
        }

        FILE *file = fopen(_filePath.data(), "rb");

        if (file)
        {
            struct stat fileStat;
            stat(_filePath.data(), &fileStat);
            int fd = fileno(file);

            evbuffer_add_printf(buf,

                                "Content-Type: %s\r\n"
                                "Content-Length: %ld\r\n"
                                "\r\n",

                                contentTypeString.data(),
                                fileStat.st_size
            );

            if (GetMethod() != RequestMethod::Head)
            {
                if (evbuffer_add_file(buf, fd, 0, fileStat.st_size) == -1)
                {
                    fclose(file);
                }
            }
        }
    } else {
        const char *errMsg = GetCodeMsg();

        evbuffer_add_printf(buf,

                            "Content-Type: text/plain\n"
                            "Content-Length: %zi\n"
                            "\n",

                            strlen(errMsg)
        );

        if (_requestMethod != RequestMethod::Head)
        {
            evbuffer_add_printf(buf,

                                "Error %d: %s",

                                GetCode(), errMsg
            );
        }
    }

}