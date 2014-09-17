#ifndef WORKERS_POOL_H
#define WORKERS_POOL_H

#include "TaskItem.h"
#include "Worker.h"
#include <vector>
#include <map>
#include <mutex>
#include <algorithm>
using namespace std;

class Worker;

class WorkersPool
{
public:
    enum Error
    {
        None,
        NoWorkers,
        NoTask
    };

    WorkersPool();
    ~WorkersPool();

    Worker* CreateWorker();
    bool EraseWorker(Worker *worker);
    static void DestroyWorker(Worker *worker);

    Error AddTask(TaskItem *task);
    Error RemoveTask(bufferevent *bufEv, bool force);
private:

    mutex _workersMutex;
    vector<Worker*> _workers;

    mutex _allTasksMutex;
    map<bufferevent*, TaskItem*> _allTasks;
};


#endif