#ifndef WORKER_H
#define WORKER_H

class WorkersPool;

#include "TaskItem.h"
#include <mutex>
#include <queue>
using namespace std;


class Worker
{
public:
    void AddTask(TaskItem *task);
    void UpdateTasks();
    void SetLastCycleTime(int ms);
    int GetLastCycleTime();
private:
    Worker();
    Worker(const Worker &);
    ~Worker();

    mutex _tasksMutex;
    queue<TaskItem*> _tasks;

    mutex _newTasksMutex;
    queue<TaskItem*> _newTasks;

    mutex _lastCycleTimeMSMutex;
    int _lastCycleTimeMS = 0;

    friend class WorkersPool;
};

#endif