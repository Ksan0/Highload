#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <string.h>
#include <errno.h>

#include "Http/HttpResponse.h"
#include "Http/HttpRequest.h"
#include "Server/TaskItem.h"
#include "Server/Worker.h"

#include <unistd.h>
#include <thread>
#include <iostream>
#include <signal.h>

using namespace std;


void WorkerThread(Worker *worker)
{
    while(!worker->GetExitFlag())
    {
        cout << "Update now" << endl;
        worker->UpdateTasks();
        cout << "Execute now" << endl;
        worker->ExecuteTasks();
        cout << "IsFree now" << endl;
        if (worker->IsFree()) {
            cout << "Sleep now" << endl;
            this_thread::__sleep_for(chrono::seconds(0), chrono::nanoseconds(1000));
        }
        cout << "New loop now" << endl;
    }

    delete worker;
}

void write_event_cb(bufferevent *buf_ev, void *arg)
{
    (*(Worker**)arg)->RemoveTask(buf_ev, false);
}

void read_event_cb(bufferevent *buf_ev, void *arg)
{

}


void echo_event_cb( bufferevent *buf_ev, short events, void *arg )
{
    if (events & (BEV_EVENT_EOF | BEV_EVENT_TIMEOUT | BEV_EVENT_ERROR))
    {
        (*(Worker**)arg)->RemoveTask(buf_ev, true);
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
    bufferevent_setcb( buf_ev, read_event_cb, write_event_cb, echo_event_cb, arg );
    bufferevent_enable( buf_ev, (EV_READ | EV_WRITE) );

    (*(Worker**)arg)->AddTask(new TaskItem(buf_ev));
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
    cout << "libevent version " << event_get_version() << endl;

    if (evthread_use_pthreads() == -1)
    {
        cout << "init pthreads failed" << endl;
    }
    srand((unsigned int)time(0));

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
    sin.sin_port = htons( ServerConfig::GetInstance()->GetListenPort() );

    listener = evconnlistener_new_bind( base, nullptr, nullptr,
            (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE),
            -1, (sockaddr *)&sin, sizeof(sin) );
    if( !listener )
    {
        cerr    << "Ошибка при создании объекта evconnlistener (port="
                << ServerConfig::GetInstance()->GetListenPort() << ")" << endl;
        return -1;
    }

    bool masterProcess = true;
    vector<pid_t> pids;
    for (int i = 0; i < ServerConfig::GetInstance()->GetThreadsCount() - 1; ++i)
    {
        int res = fork();
        if (res == 0) {
            masterProcess = false;
            break;
        } else if (res == -1) {
            cout << "fork error" << endl;
        } else {
            pids.push_back(res);
        }
    }

    if (event_reinit(base) == -1)
    {
        cout << "reinit error" << endl;
    }

    Worker *worker = new Worker(0);

    evconnlistener_set_error_cb( listener, accept_error_cb );
    evconnlistener_set_cb(listener, accept_connection_cb, worker);

    thread workerThread (WorkerThread, worker);

    event_base_dispatch( base );

    if (masterProcess) {
        for (int i = 0; i < pids.size(); ++i) {
            kill(pids[i], SIGKILL);
        }
    }

    worker->ExitThread();
    workerThread.join();

    return 0;
}