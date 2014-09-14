#include "WorkersPool.h"


WorkersPool::WorkersPool()
{
}


WorkersPool::~WorkersPool()
{
    _workersMutex.lock();
    for (auto iter = _workers.begin(); iter != _workers.end(); iter++)
    {
        delete (*iter);
    }
    _workers.clear();
    _workersMutex.unlock();
}


Worker* WorkersPool::CreateWorker()
{
    Worker *newWorker = new Worker;
    _workersMutex.lock();
    _workers.push_back(newWorker);
    _workersMutex.unlock();
    return newWorker;
}


bool WorkersPool::DestroyWorker(Worker *worker)
{
    _workersMutex.lock();
    auto iter = find(_workers.begin(), _workers.end(), worker);
    if (iter != _workers.end())
    {
        _workers.erase(iter);
        _workersMutex.unlock();
        delete worker;
        return true;
    }
    _workersMutex.unlock();
    return false;
}


WorkersPool::Error WorkersPool::AddTask(TaskItem *task)
{
    _workersMutex.lock();
    if (_workers.empty())
    {
        _workersMutex.unlock();
        return Error::NoWorkers;
    }

    int minCycleTimeMSIndex = 0;
    int minCycleTimeMS = _workers[0]->GetLastCycleTime();
    int tmpCycleTimeMS;
    for (int i  = 1; i < _workers.size(); ++i)
    {
        tmpCycleTimeMS = _workers[i]->GetLastCycleTime();
        if (tmpCycleTimeMS < minCycleTimeMS)
        {
            minCycleTimeMS = tmpCycleTimeMS;
            minCycleTimeMSIndex = i;
        }
    }

    _workers[minCycleTimeMSIndex]->AddTask(task);

    _workersMutex.unlock();

    return Error::None;
}