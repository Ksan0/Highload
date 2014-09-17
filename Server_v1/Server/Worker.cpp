#include "Worker.h"

#include <time.h>
#include <iostream>

Worker::Worker()
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


void Worker::RemoveTask(TaskItem *task)
{
    _newTasksMutex.lock();
    _newTasks.push(new TaskItemAction(task, TaskItemAction::Action::Remove));
    _newTasksMutex.unlock();
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
            _tasks.push_back(taskItemAction->item);
            break;
        case TaskItemAction::Action::Remove:
            auto iter = find(_tasks.begin(), _tasks.end(), taskItemAction->item);
            if (iter != _tasks.end())
            {
                delete (*iter);
                _tasks.erase(iter);
            }
            break;
        }

        delete taskItemAction;
    }

    _newTasksMutex.unlock();
    _tasksMutex.unlock();
}


void Worker::ExecuteTasks()
{
    time_t tBegin = time(NULL);

    _tasksMutex.lock();
    for (auto iter = _tasks.begin(); iter != _tasks.end(); iter++)
    {
        (*iter)->Execute();
    }
    _tasksMutex.unlock();

    double dt = difftime(time(NULL), tBegin);

    _lastExecuteTimeMSMutex.lock();
    _lastExecuteTimeMS = (int)(dt * 1000);
    _lastExecuteTimeMSMutex.unlock();
}


bool Worker::IsFree()
{
    _tasksMutex.lock();
    bool empty = _tasks.empty();
    _tasksMutex.unlock();
    return empty;
}


int Worker::GetLastExecuteTime()
{
    _lastExecuteTimeMSMutex.lock();
    int ms = _lastExecuteTimeMS;
    _lastExecuteTimeMSMutex.unlock();
    return ms;
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