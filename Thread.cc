#include "Thread.hh"

Thread::Thread(ThreadFunc func, int id)
    : f_(func),
    tid_(id)
{}

Thread::~Thread()
{}

void Thread::start()
{
    std::thread t(f_, tid_);
    t.detach();
    
}
