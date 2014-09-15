#ifndef TASK_ITEM_EXECUTER
#define TASK_ITEM_EXECUTER

#include <event2/bufferevent.h>
#include <event2/buffer.h>

class TaskItemExecuter
{
public:
    void Execute(evbuffer *readBuf, evbuffer *writeBuf);
private:

};


#endif