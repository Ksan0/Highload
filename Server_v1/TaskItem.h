#include <event2/bufferevent_struct.h>
#include <mutex>
using namespace std;

class TaskItem
{
public:
    TaskItem(bufferevent *buf_ev)
    {
        _buf_ev = buf_ev;
        _new_info = true;
    }
    bufferevent* get_bufferevent()
    {
        return _buf_ev;
    }
private:
    TaskItem();
    TaskItem(const TaskItem &);

    mutex _mutex;
    bool _new_info;
    bufferevent *_buf_ev;
};