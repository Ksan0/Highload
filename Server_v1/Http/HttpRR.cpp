#include "HttpRR.h"


HttpRR::HttpRR()
{
}


void HttpRR::SetMethod(RequestMethod m)
{
    _requestMethod = m;
}


HttpRR::RequestMethod HttpRR::GetMethod()
{
    return _requestMethod;
}


void HttpRR::SetHeader(const string &key, const string &value)
{
    _headers.insert(pair<string, string>(key, value));
}


bool HttpRR::GetHeader(const string &key, string &value)
{
    auto iter = _headers.find(key);
    if (iter != _headers.end())
    {
        value = iter->second;
        return true;
    }
    return false;
}


void HttpRR::SetVersion(float v)
{
    _version = v;
}


float HttpRR::GetVersion()
{
    return _version;
}


void HttpRR::SetUri(const string &uri)
{
    _uri = uri;
}


string HttpRR::GetUri()
{
    return _uri;
}