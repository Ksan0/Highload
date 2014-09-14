#ifndef TASK_ITEM_H
#define TASK_ITEM_H

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <mutex>
using namespace std;

class TaskItem
{
public:
    TaskItem(bufferevent *buf_ev);
    void Read(evbuffer *data);
    void Write(evbuffer *data);
private:
    TaskItem();
    TaskItem(const TaskItem &);

    bufferevent *_buf_ev;
};

#endif