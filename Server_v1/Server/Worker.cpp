#include "Worker.h"

#include <iostream>

Worker::Worker(int id) : _id(id)
{
}


Worker::~Worker()
{
}


void Worker::AddTask(TaskItem *task)
{
    _newTasksMutex.lock();
    _newTasks.push(new TaskItemAction(task, TaskItemAction::Action::Add));
    _newTasksMutex.unlock();
}


void Worker::RemoveTask(bufferevent *bufEvTask, bool force)
{
    _newTasksMutex.lock();
    _newTasks.push(new TaskItemAction(new TaskItemAction::__RemoveAction(bufEvTask, force), TaskItemAction::Action::Remove));
    _newTasksMutex.unlock();
}


void Worker::__AddAction(TaskItem *add)
{
    _tasks.push_back(add);
}


void Worker::__RemAction(bufferevent *bufEv, bool force)
{
    for(auto iter = _tasks.begin(); iter != _tasks.end(); ++iter)
    {
        if ((*iter)->GetBufferEvent() == bufEv)
        {
            if ((*iter)->GetFinishFlag() || force)
            {
                delete (*iter);
                _tasks.erase(iter);
            } else {
                break;
            }
        }
    }
}


void Worker::UpdateTasks()
{
    _tasksMutex.lock();
    _newTasksMutex.lock();

    while(!_newTasks.empty())
    {
        TaskItemAction *taskItemAction = _newTasks.front();
        _newTasks.pop();

        switch (taskItemAction->action)
        {
        case TaskItemAction::Action::Add:
            __AddAction((TaskItem*)taskItemAction->item);
            break;
        case TaskItemAction::Action::Remove:
            __RemAction( ((TaskItemAction::__RemoveAction*)taskItemAction->item)->item,
                         ((TaskItemAction::__RemoveAction*)taskItemAction->item)->force);
            delete (TaskItemAction::__RemoveAction*)taskItemAction->item;
            break;
        }

        delete taskItemAction;
    }

    _newTasksMutex.unlock();
    _tasksMutex.unlock();
}


void Worker::ExecuteTasks()
{
    _tasksMutex.lock();
    for (auto iter = _tasks.begin(); iter != _tasks.end(); iter++)
    {
        (*iter)->Execute();
    }
    _tasksMutex.unlock();
}


bool Worker::IsFree()
{
    _tasksMutex.lock();
    bool empty = _tasks.empty();
    _tasksMutex.unlock();
    return empty;
}


void Worker::ExitThread()
{
    _exitWorkerThreadMutex.lock();
    _exitWorkerThread = true;
    _exitWorkerThreadMutex.unlock();
}


bool Worker::GetExitFlag()
{
    _exitWorkerThreadMutex.lock();
    bool flag = _exitWorkerThread;
    _exitWorkerThreadMutex.unlock();
    return flag;
}