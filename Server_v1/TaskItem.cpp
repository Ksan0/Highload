#include "TaskItem.h"


TaskItem::TaskItem(bufferevent *buf_ev)
{
    _buf_ev = buf_ev;
}


void TaskItem::Read(evbuffer *data)
{
    bufferevent_lock(_buf_ev);
    bufferevent_read_buffer(_buf_ev, data);
    bufferevent_unlock(_buf_ev);
}


void TaskItem::Write(evbuffer *data)
{
    bufferevent_lock(_buf_ev);
    bufferevent_write_buffer(_buf_ev, data);
    bufferevent_unlock(_buf_ev);
}