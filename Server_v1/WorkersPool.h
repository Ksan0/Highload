#ifndef WORKERS_POOL_H
#define WORKERS_POOL_H

#include "TaskItem.h"
#include "Worker.h"
#include <vector>
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
        NoWorkers
    };

    WorkersPool();
    ~WorkersPool();

    Worker* CreateWorker();
    bool DestroyWorker(Worker *worker);

    Error AddTask(TaskItem *task);
private:

    mutex _workersMutex;
    vector<Worker*> _workers;

};


#endif