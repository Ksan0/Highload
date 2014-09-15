#ifndef WORKER_H
#define WORKER_H

class WorkersPool;

#include "TaskItem.h"
#include <mutex>
#include <queue>
#include <algorithm>
using namespace std;

class TaskItem;

class Worker
{
public:
    void AddTask(TaskItem *task);
    void RemoveTask(TaskItem *task);
    void UpdateTasks();
    void ExecuteTasks();

    bool IsFree();

    int GetLastExecuteTime();

    void ExitThread();
    bool GetExitFlag();
private:
    Worker();
    Worker(const Worker &);
    ~Worker();

    mutex _exitWorkerThreadMutex;
    bool _exitWorkerThread = false;

    mutex _tasksMutex;
    vector<TaskItem*> _tasks;

    struct TaskItemAction
    {
        enum Action
        {
            Add,
            Remove
        };

        TaskItemAction(TaskItem *item, Action action)
        {
            this->item = item;
            this->action = action;
        }

        TaskItem *item;
        Action action;
    };
    mutex _newTasksMutex;
    queue<TaskItemAction*> _newTasks;

    mutex _lastExecuteTimeMSMutex;
    int _lastExecuteTimeMS = 0;

    friend class WorkersPool;
};

#endif