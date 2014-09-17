#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "HttpRR.h"
#include <event2/buffer.h>
#include <string>
#include <map>
using namespace std;

class HttpResponse : public HttpRR
{
public:
    HttpResponse();

    void SetCode(int code);
    bool IsCodeDefined();
    int GetCode();

    void SetFilePath(const string &path, const string &extension);

    void WriteToBuffer(evbuffer *buf);
private:
    const char* GetCodeMsg();

    string _filePath;
    string _extension;

    int _code;
};


#endif