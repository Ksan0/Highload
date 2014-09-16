#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H


#include "../Server/ServerConfig.h"
#include "HttpRR.h"
#include "HttpResponse.h"
#include <string>
using namespace std;


class HttpRequest : protected HttpRR
{
public:
    enum RequestMethod
    {
        None,
        Head,
        Get
    };

    HttpRequest();

    void AddLine(char *line, size_t len, HttpResponse &response);
    void HeadersEnd();

    bool IsReadyToProcess();
    void Process(HttpResponse &response);

private:
    void _AddLineStartingLine(char *line, size_t len, HttpResponse &response);
    void _AddLineHeaders(char *line, size_t len, HttpResponse &response);

    enum ParseStatus
    {
        StartingLine,
        Headers,
        MessageBody
    };

    ParseStatus _parseStatus;

    RequestMethod _requestMethod;
};


#endif