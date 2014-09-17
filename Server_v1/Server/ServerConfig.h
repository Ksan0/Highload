#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>
using namespace std;

class ServerConfig
{
public:
    static ServerConfig* GetInstance();

    int GetThreadsCount();
    unsigned short GetListenPort();
    string GetDocumentRoot();
private:
    ServerConfig();
    ServerConfig(const ServerConfig &);
    void _Load();

    static ServerConfig *__instance;

    int _threadsCount;
    unsigned short _listenPort;
    string _documentRoot;
};


#endif