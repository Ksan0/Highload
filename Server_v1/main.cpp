#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <string.h>
#include <errno.h>

// ???
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"
#include "Server/TaskItem.h"
#include "Server/Worker.h"
#include "Server/WorkersPool.h"

#include <unistd.h>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <queue>
#include <map>
using namespace std;


void WorkerThread(WorkersPool *workersPool)
{
    Worker *worker = workersPool->CreateWorker();

    while(!worker->GetExitFlag())
    {
        worker->UpdateTasks();
        worker->ExecuteTasks();
        if (worker->IsFree())
            usleep(1000);
    }

    WorkersPool::DestroyWorker(worker);
}

void write_event_cb(bufferevent *buf_ev, void *arg)
{
    auto len = evbuffer_get_length(bufferevent_get_output(buf_ev));

    if (len > 0)
        cout << "l " << len << endl;
    ((WorkersPool*)arg)->RemoveTask(buf_ev, false);
}


void echo_event_cb( bufferevent *buf_ev, short events, void *arg )
{
    if (events & (BEV_EVENT_EOF | BEV_EVENT_TIMEOUT | BEV_EVENT_ERROR))
    {
        ((WorkersPool*)arg)->RemoveTask(buf_ev, true);
    }
}

void accept_connection_cb( evconnlistener *listener,
        evutil_socket_t fd, sockaddr *addr, int sock_len,
        void *arg )
{

    event_base *base = evconnlistener_get_base( listener );
    bufferevent *buf_ev = bufferevent_socket_new( base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE );

    timeval timeout;
    timeout.tv_usec = 0;
    timeout.tv_sec = 30;

    bufferevent_set_timeouts( buf_ev, &timeout, &timeout);
    bufferevent_setcb( buf_ev, NULL, write_event_cb, echo_event_cb, arg );
    bufferevent_enable( buf_ev, (EV_READ | EV_WRITE) );

    ((WorkersPool*)arg)->AddTask(new TaskItem(buf_ev));
}

void accept_error_cb( evconnlistener *listener, void *arg )
{
    event_base *base = evconnlistener_get_base( listener );
    int error = EVUTIL_SOCKET_ERROR();
    cerr    << "Ошибка " << error
            << " (" << evutil_socket_error_to_string( error ) << ")"
            << " в мониторе соединений. Завершение работы." << endl;
    event_base_loopexit( base, NULL );
}


int main( int argc, char **argv )
{
    evthread_use_pthreads();

    WorkersPool *workersPool;
    vector<thread*> threads;

    event_base *base;
    evconnlistener *listener;
    sockaddr_in sin;

    base = event_base_new();
    if( !base )
    {
        cerr << "Ошибка при создании объекта event_base" << endl;
        return -1;
    }

    memset( &sin, 0, sizeof(sin) );
    sin.sin_family = AF_INET;    /* работа с доменом IP-адресов */
    sin.sin_addr.s_addr = htonl( INADDR_ANY );  /* принимать запросы с любых адресов */
    sin.sin_port = htons( ServerConfig::GetInstance()->GetListenPort());

    workersPool = new WorkersPool;
    listener = evconnlistener_new_bind( base, accept_connection_cb, workersPool,
            (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE),
            -1, (sockaddr *)&sin, sizeof(sin) );
    if( !listener )
    {
        cerr    << "Ошибка при создании объекта evconnlistener (port="
                << ServerConfig::GetInstance()->GetListenPort() << ")" << endl;
        delete workersPool;
        return -1;
    }
    evconnlistener_set_error_cb( listener, accept_error_cb );

    for (int i = 0; i < ServerConfig::GetInstance()->GetThreadsCount(); ++i)
    {
        thread *newThread = new thread(WorkerThread, workersPool);
        threads.push_back(newThread);
    }

    event_base_dispatch( base );

    delete workersPool;
    for (auto iter = threads.begin(); iter != threads.end(); iter++)
    {
        (*iter)->join();
        delete (*iter);
    }

    return 0;
}