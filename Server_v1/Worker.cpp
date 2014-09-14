#include "Worker.h"


void Worker::AddTask(TaskItem *task)
{
    _newTasksMutex.lock();
    _newTasks.push(task);
    _newTasksMutex.unlock();
}


void Worker::UpdateTasks()
{
    _tasksMutex.lock();
    _newTasksMutex.lock();

    while(!_newTasks.empty())
    {
        _tasks.push(_newTasks.front());
        _newTasks.pop();
    }

    _newTasksMutex.unlock();
    _tasksMutex.unlock();
}


void Worker::SetLastCycleTime(int ms)
{
    _lastCycleTimeMSMutex.lock();
    _lastCycleTimeMS = ms;
    _lastCycleTimeMSMutex.unlock();
}


int Worker::GetLastCycleTime()
{
    _lastCycleTimeMSMutex.lock();
    int ms = _lastCycleTimeMS;
    _lastCycleTimeMSMutex.unlock();
    return ms;
}


Worker::Worker()
{
}


Worker::~Worker()
{
}
