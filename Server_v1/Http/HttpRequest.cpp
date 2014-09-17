#include "HttpRequest.h"

#include <string.h>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <iostream>


string __UrlDecode(const string &src)
{
    string ret;
    char ch;
    int i, ii;
    for (i = 0; i < src.length(); ++i) {
        if (src[i] == 37)
        {
            sscanf(src.data() + i + 1, "%x", &ii);
            ch = (char)ii;
            ret += ch;
            i = i + 2;
        } else {
            ret += src[i];
        }
    }
    return ret;
}


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
    string fileName = GetPage();
    if (fileName.length() > 1 && fileName.back() == '/')
    {
        fileName += "index.html";
    }
    fileName = __UrlDecode(fileName);

    string extension;
    for (int i = 0; i < fileName.length(); ++i)
    {
        char ch = fileName[fileName.length() - i - 1];
        if (ch == '.' || ch == '/')
        {
            break;
        } else {
            extension += ch;
        }
    }
    reverse(extension.begin(), extension.end());

    string path = ServerConfig::GetInstance()->GetDocumentRoot() + fileName;
    char *realPath = realpath(path.data(), nullptr);

    if (realPath)
    {
        string strRealPath = realPath;

        bool insideDocRoot = strRealPath.find(ServerConfig::GetInstance()->GetDocumentRoot()) != string::npos;
        bool haveAccess = access(realPath, R_OK) != -1;
        if (insideDocRoot && haveAccess)
        {
            response.SetFilePath(path, extension);
            response.SetCode(200);
        } else {
            response.SetCode(404);
        }

        free(realPath);
    } else {
        response.SetCode(404);
    }

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
            HeadersEnd();
        }
        break;
    case ParseStatus::MessageBody:
        // isn't support in current version of server
        break;
    }
}


void HttpRequest::HeadersEnd()
{
    _parseStatus = ParseStatus::MessageBody;
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
        } else if (strcmp(args[0], "GET") == 0)
        {
            _requestMethod = RequestMethod::Get;
        } else {
            response.SetCode(405);
        }
        response.SetMethod(_requestMethod);
    }

    if (args.size() >= 2)
    {
        SetUri(args[1]);
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