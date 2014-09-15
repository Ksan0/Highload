#ifndef TASK_ITEM_H
#define TASK_ITEM_H

#include "TaskItemExecuter.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <mutex>
using namespace std;

class Worker;

class TaskItem
{
public:
    TaskItem(bufferevent *bufEv);
    ~TaskItem();

    bufferevent* GetBufferEvent();

    void SetWorker(Worker *worker);
    Worker* GetWorker();

    void Execute();
private:
    TaskItem();
    TaskItem(const TaskItem &);

    bufferevent *_bufEv;
    Worker *_worker;

    evbuffer *_readBuf;
    evbuffer *_writeBuf;

    TaskItemExecuter _executer;
};

#endif