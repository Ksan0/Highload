#include "TaskItemExecuter.h"


void TaskItemExecuter::Execute(evbuffer *readBuf, evbuffer *writeBuf)
{
    char response[] =   "HTTP/1.0 200 OK\n"
                        "Content-Type: text/html\n"
                        "Content-Length: 6\n"
                        "Connection: close\n"
                        "\n"
                        "aswdqe";
    evbuffer_add(writeBuf, response, sizeof(response));
}