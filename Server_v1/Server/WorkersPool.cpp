#include "WorkersPool.h"


WorkersPool::WorkersPool()
{
}


WorkersPool::~WorkersPool()
{
    _workersMutex.lock();
    for (auto iter = _workers.begin(); iter != _workers.end(); iter++)
    {
        (*iter)->ExitThread();
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


bool WorkersPool::EraseWorker(Worker *worker)
{
    _workersMutex.lock();
    auto iter = find(_workers.begin(), _workers.end(), worker);
    if (iter != _workers.end())
    {
        _workers.erase(iter);
        _workersMutex.unlock();
        worker->ExitThread();
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

    int minExecuteTimeMSIndex = 0;
    int minExecuteTimeMS = _workers[0]->GetLastExecuteTime();
    int tmpExecuteTimeMS;
    for (int i  = 1; i < _workers.size(); ++i)
    {
        tmpExecuteTimeMS = _workers[i]->GetLastExecuteTime();
        if (tmpExecuteTimeMS < minExecuteTimeMS)
        {
            minExecuteTimeMS = tmpExecuteTimeMS;
            minExecuteTimeMSIndex = i;
        }
    }

    task->SetWorker(_workers[minExecuteTimeMSIndex]);
    _workers[minExecuteTimeMSIndex]->AddTask(task);

    _workersMutex.unlock();

    _allTasksMutex.lock();
    _allTasks.insert(pair<bufferevent*, TaskItem*>(task->GetBufferEvent(), task));
    _allTasksMutex.unlock();

    return Error::None;
}

WorkersPool::Error WorkersPool::RemoveTask(bufferevent *bufEv, bool force)
{
    _allTasksMutex.lock();
    auto iter = _allTasks.find(bufEv);
    if (iter != _allTasks.end())
    {
        TaskItem *item = iter->second;
        if (force || item->GetFinishFlag())
        {
            Worker *worker = item->GetWorker();
            if (worker != nullptr)
            {
                worker->RemoveTask(item);
            }
            _allTasks.erase(iter);
        }
    }
    _allTasksMutex.unlock();
}

void WorkersPool::DestroyWorker(Worker *worker)
{
    delete worker;
}