#include "TaskItem.h"

#include <iostream>

TaskItem::TaskItem(bufferevent *bufEv)
{
    _bufEv = bufEv;
    _worker = nullptr;
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

void TaskItem::Execute()
{
    std::cout << "EXE ME!" << std::endl;
}