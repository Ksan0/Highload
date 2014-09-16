#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>
using namespace std;

class ServerConfig
{
public:
    static ServerConfig* GetInstance();

    string GetDocumentRoot();
private:
    ServerConfig();
    ServerConfig(const ServerConfig &);
    void _Load();

    static ServerConfig *__instance;

    string _documentRoot;
};


#endif