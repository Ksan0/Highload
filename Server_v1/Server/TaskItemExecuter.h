#ifndef TASK_ITEM_EXECUTER
#define TASK_ITEM_EXECUTER

#include "../Http/HttpRequest.h"
#include "../Http/HttpResponse.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class TaskItemExecuter
{
public:
    void Execute(evbuffer *readBuf, evbuffer *writeBuf, bool *finishFlag    );
private:
    HttpRequest _request;
    HttpResponse _response;
};


#endif