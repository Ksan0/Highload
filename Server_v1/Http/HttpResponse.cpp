#include "HttpResponse.h"
#include <sstream>
#include <iomanip>



map<int, const char*> __InitCodeToMsg()
{
    map<int, const char*> dict;
    dict.insert(pair<int, const char*>(200, "OK"));
    dict.insert(pair<int, const char*>(404, "Not Found"));
    dict.insert(pair<int, const char*>(405, "Method Not Allowed"));
    return dict;
}
const map<int, const char*> HttpResponse::_codeToMsg = __InitCodeToMsg();


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
    auto iter = _codeToMsg.find(_code);
    if (iter != _codeToMsg.end())
    {
        return iter->second;
    }
    return "";
}


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

    /*sprintf(result, ,
            wday_name[timeptr->tm_wday],
            timeptr->tm_mday,
            mon_name[timeptr->tm_mon],
            1900 + timeptr->tm_year,
            timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);*/

    evbuffer_add_printf(buf,

                        "HTTP/%.1f %d %s\n"
                        "Date: %.3s,%3d %.3s %d %.2d:%.2d:%.2d GMT\n"
                        "Server: Ksan.Server.v0.01\n"
                        "Connection: close\n"
                        "\n",

                        GetVersion(), GetCode(), GetCodeMsg(),
                        wday_name[timeptr->tm_wday], timeptr->tm_mday, mon_name[timeptr->tm_mon], 1900 + timeptr->tm_year, timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec

            );
}