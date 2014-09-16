#ifndef HTTP_RR_H
#define HTTP_RR_H

#include <string>
#include <map>
using namespace std;

class HttpRR
{
public:
    HttpRR();

    void SetHeader(const string &key, const string &value);
    bool GetHeader(const string &key, string &value);

    void SetVersion(float v);
    float GetVersion();

    void SetUri(const string &uri);
    string GetUri();
protected:
    map<string, string> _headers;
    string _uri;
    float _version;
};


#endif