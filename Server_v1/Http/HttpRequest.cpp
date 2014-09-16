#include "HttpRequest.h"

#include <string.h>
#include <vector>
#include <iostream>

static void __SplitLine(char *line, const char *delim, vector<char*> &out_args)
{
    char *context = nullptr;
    char *ptr = strtok_r(line, delim, &context);
    while(ptr)
    {
        out_args.push_back(ptr);
        ptr = strtok_r(nullptr, delim, &context);
    }
}


HttpRequest::HttpRequest()
{
    _parseStatus = ParseStatus::StartingLine;

    _requestMethod = RequestMethod::None;
    _version = 0.0f;
}


bool HttpRequest::IsReadyToProcess()
{
    return _parseStatus == ParseStatus::MessageBody;
}


void HttpRequest::Process(HttpResponse &response)
{
    if (_uri == "/")
    {
        response.SetFilePath("index.html");
        return;
    }

    string path = ServerConfig::GetInstance()->GetDocumentRoot();
    path += _uri;


}


void HttpRequest::AddLine(char *line, size_t len, HttpResponse &response)
{
    switch(_parseStatus)
    {
    case ParseStatus::StartingLine:
        _AddLineStartingLine(line, len, response);
        _parseStatus = ParseStatus::Headers;
        break;
    case ParseStatus::Headers:
        if (len > 0)
        {
            _AddLineHeaders(line, len, response);
        } else {
            _parseStatus = ParseStatus::Headers;
        }
        break;
    case ParseStatus::MessageBody:
        // isn't support in current version of server
        break;
    }
}


void HttpRequest::_AddLineStartingLine(char *line, size_t len, HttpResponse &response)
{
    vector<char*> args;
    __SplitLine(line, " ", args);

    if (args.size() >= 1)
    {
        if (strcmp(args[0], "HEAD") == 0)
        {
            _requestMethod = RequestMethod::Head;
        } else if (strcmp(args[0], "GET"))
        {
            _requestMethod = RequestMethod::Get;
        }
    }

    if (args.size() >= 2)
    {
        _uri = args[1];
    }

    if (args.size() >= 3)
    {
        const size_t httpPartLen = strlen("HTTP/");
        if (strlen(args[2]) > httpPartLen)
        {
            float version = (float)atof(args[2] + httpPartLen);
            SetVersion(version);
            response.SetVersion(version);
        }
    }

    if (args.size() >= 4)
    {
        response.SetCode(405);
    }
}


void HttpRequest::_AddLineHeaders(char *line, size_t len, HttpResponse &response)
{
    vector<char*> args;
    __SplitLine(line, ": ", args);

    if (args.size() == 2)
    {
        SetHeader(args[0], args[1]);
    } else {
        response.SetCode(405);
    }
}