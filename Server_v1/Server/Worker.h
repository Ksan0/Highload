#ifndef WORKER_H
#define WORKER_H

#include "TaskItem.h"
#include <mutex>
#include <queue>
#include <algorithm>
using namespace std;

class TaskItem;

class Worker
{
public:
    Worker(int id);
    ~Worker();

    void AddTask(TaskItem *task);
    void RemoveTask(bufferevent *bufEvTask, bool force);
    void UpdateTasks();
    void ExecuteTasks();

    bool IsFree();

    void ExitThread();
    bool GetExitFlag();
private:
    Worker(const Worker &);
    void __AddAction(TaskItem *add);
    void __RemAction(bufferevent *bufEv, bool force);

    const int _id;

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

        TaskItemAction(void *item, Action action)
        {
            this->item = item;
            this->action = action;
        }

        void *item;
        Action action;

        struct __RemoveAction
        {
            __RemoveAction(bufferevent *item, bool force)
            {
                this->item = item;
                this->force = force;
            };

            bufferevent *item;
            bool force;
        };
    };

    mutex _newTasksMutex;
    queue<TaskItemAction*> _newTasks;
};

#endif