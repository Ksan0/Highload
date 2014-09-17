#include "TaskItem.h"

#include <iostream>
#include <event2/event.h>
#include <unistd.h>

using namespace std;


TaskItem::TaskItem(bufferevent *bufEv)
{
    _bufEv = bufEv;
    _worker = nullptr;

    _readBuf = evbuffer_new();
    _writeBuf = evbuffer_new();

    _finishFlag = false;
}


TaskItem::~TaskItem()
{
    bufferevent_free(_bufEv);

    evbuffer_free(_readBuf);
    evbuffer_free(_writeBuf);
}


bufferevent* TaskItem::GetBufferEvent()
{
    return _bufEv;
}


void TaskItem::SetWorker(Worker *worker)
{
    _worker = worker;
}

Worker* TaskItem::GetWorker()
{
    return _worker;
}


bool TaskItem::GetFinishFlag()
{
    return _finishFlag;
}


void TaskItem::Execute()
{
    bufferevent_lock(_bufEv);
    bufferevent_read_buffer(_bufEv, _readBuf);
    bufferevent_unlock(_bufEv);

    bool finishFlag;
    _executer.Execute(_readBuf, _writeBuf, &finishFlag);

    bufferevent_lock(_bufEv);
    bufferevent_write_buffer(_bufEv, _writeBuf);
    bufferevent_unlock(_bufEv);

    if (finishFlag)
    {
        _finishFlag = finishFlag;
    }
}