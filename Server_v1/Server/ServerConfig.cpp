#include "ServerConfig.h"

ServerConfig *ServerConfig::__instance = nullptr;

ServerConfig* ServerConfig::GetInstance()
{
    if (__instance == nullptr)
    {
        __instance = new ServerConfig;
        __instance->_Load();
    }

    return __instance;
}

ServerConfig::ServerConfig()
{
}


void ServerConfig::_Load()
{
    _threadsCount = 2;
    _listenPort = 9876;
    _documentRoot = "/home/ksan/TP/Highload/Server_v1/document_root";
}


int ServerConfig::GetThreadsCount()
{
    return _threadsCount;
}


unsigned short ServerConfig::GetListenPort()
{
    return _listenPort;
}


string ServerConfig::GetDocumentRoot()
{
    return _documentRoot;
}