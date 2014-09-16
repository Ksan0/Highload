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
    _documentRoot = "/home/ksan/TP/Highload/Server_v1";
}


string ServerConfig::GetDocumentRoot()
{
    return _documentRoot;
}