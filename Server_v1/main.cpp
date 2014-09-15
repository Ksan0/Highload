#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <string.h>
#include <errno.h>

// ???
#include "WorkersPool.h"
#include "Worker.h"
#include "TaskItem.h"

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


/*void foo()
{
    char chbuf[1024*16];
    while(loop_var)
    {
        vec_mutex.lock();
        for(auto iter = vec.begin(); iter != vec.end(); iter++) {
            evbuffer *buf_in = evbuffer_new();
            bufferevent_lock(*iter);
            if (!bufferevent_read_buffer(*iter, buf_in)) {

                int readed = evbuffer_remove(buf_in, chbuf, sizeof(chbuf));
                chbuf[readed] = 0;
            }

            //char response[] = "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: 3\nConnection: close\n\nfff";
            //evbuffer_add(buf_in, response, sizeof(response));

            //bufferevent_write_buffer(*iter, buf_in);

            /*evbuffer *buf_input = bufferevent_get_input( *iter );
            evbuffer *buf_output = bufferevent_get_output( *iter );
            evbuffer_add_buffer( buf_output, buf_input ); /
            bufferevent_unlock(*iter);
            evbuffer_free(buf_in);
        }
        vec_mutex.unlock();

        usleep(1000);
    }
}*/


void echo_event_cb( bufferevent *buf_ev, short events, void *arg )
{
    if (events & (BEV_EVENT_EOF | BEV_EVENT_TIMEOUT | BEV_EVENT_ERROR))
    {
        ((WorkersPool*)arg)->RemoveTask(buf_ev);
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
    timeout.tv_sec = 300;

    bufferevent_set_timeouts( buf_ev, &timeout, &timeout);
    bufferevent_setcb( buf_ev, NULL, NULL, echo_event_cb, arg );
    bufferevent_enable( buf_ev, (EV_READ | EV_WRITE | EV_TIMEOUT) );

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
    int threadsCount = 4;
    vector<thread*> threads;

    event_base *base;
    evconnlistener *listener;
    sockaddr_in sin;
    unsigned short port = 9876;

    base = event_base_new();
    if( !base )
    {
        cerr << "Ошибка при создании объекта event_base" << endl;
        return -1;
    }

    memset( &sin, 0, sizeof(sin) );
    sin.sin_family = AF_INET;    /* работа с доменом IP-адресов */
    sin.sin_addr.s_addr = htonl( INADDR_ANY );  /* принимать запросы с любых адресов */
    sin.sin_port = htons( port );

    workersPool = new WorkersPool;
    listener = evconnlistener_new_bind( base, accept_connection_cb, workersPool,
            (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE),
            -1, (sockaddr *)&sin, sizeof(sin) );
    if( !listener )
    {
        cerr << "Ошибка при создании объекта evconnlistener" << endl;
        delete workersPool;
        return -1;
    }
    evconnlistener_set_error_cb( listener, accept_error_cb );

    for (int i = 0; i < threadsCount; ++i)
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