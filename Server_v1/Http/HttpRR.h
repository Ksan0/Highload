#ifndef HTTP_RR_H
#define HTTP_RR_H

#include <string>
#include <map>
using namespace std;

class HttpRR
{
public:
    enum RequestMethod
    {
        None,
        Head,
        Get
    };

    HttpRR();

    void SetMethod(RequestMethod m);
    RequestMethod GetMethod();

    void SetHeader(const string &key, const string &value);
    bool GetHeader(const string &key, string &value);

    void SetVersion(float v);
    float GetVersion();

    void SetUri(const string &uri);
    string GetUri();

    string GetPage();
protected:

    RequestMethod _requestMethod;
    map<string, string> _headers;
    float _version;

private:

    string _uri;

};


#endif