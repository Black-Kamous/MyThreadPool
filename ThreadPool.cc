#include "ThreadPool.hh"

ThreadPool::ThreadPool()
    : mode_(PoolMode::FIXED),
    fixedThread_(0),
    maxTask_(1024),
    stopAll_(false)
{}

void ThreadPool::stop(){
    stopAll_ = true;
    std::unique_lock<std::mutex> lock(taskListMutex_);
    notEmpty_.notify_all();
    canStop_.wait(lock, [&](){ return ( pool_.empty()); });
    std::cout << "Pool safely quited" << std::endl;
}

ThreadPool::~ThreadPool()
{
    if(!stopAll_)
        this->stop();
}

void ThreadPool::start(const int maxThread)
{
    fixedThread_ = maxThread;
    std::function<void(int)> f = std::bind(&ThreadPool::workThreadFunc, this, std::placeholders::_1);

    for(int i=0;i<maxThread;++i) {
        pool_.emplace(i, std::make_unique<Thread>(f, i));
    }

    for(int i=0;i<maxThread;++i) {
        std::cout << "= Started a thread" << std::endl;
        pool_[i]->start();
        idleThreadNum_++;
        curThread_++;
    }
    notFull_.notify_all();
}

void ThreadPool::workThreadFunc(int id)
{
    auto lastTime = std::chrono::high_resolution_clock().now();
    Task task;
    while(true){
        {
        std::unique_lock<std::mutex> lock(taskListMutex_);
        if(mode_ == PoolMode::CACHED){
            if(notEmpty_.wait_for(lock, std::chrono::seconds(1),
                [&]() {return (!taskList_.empty() || stopAll_);})
                &&
                (taskList_.empty() && !stopAll_)){
                auto curTime = std::chrono::high_resolution_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::seconds>(curTime - lastTime);
                if(dur.count() > RECOLLECT_TIME_SEC && curThread_ > fixedThread_){
                    idleThreadNum_--;
                    curThread_--;
                    pool_.erase(id);
                    std::cout << "- Thread quit on " << std::this_thread::get_id() << std::endl;
                    break;
                }
            }
        }else
        {
            notEmpty_.wait(lock, [&]() {return (!taskList_.empty() || stopAll_);});
        }
        idleThreadNum_--;
        if(!taskList_.empty()){
            std::cout << "# Get a task on " << std::this_thread::get_id() << std::endl;
            task = taskList_.front();
            taskList_.pop();
            currTaskNum_--;
        }else{
            task = nullptr;
        }

        if(!taskList_.empty())
            notEmpty_.notify_all();

        notFull_.notify_all();
        }

        if(task != nullptr) {
            task();
        }
        idleThreadNum_++;

        if(stopAll_) {
            idleThreadNum_--;
            pool_.erase(id);
            canStop_.notify_all();
            std::cout << "- Thread quit on " << std::this_thread::get_id() << std::endl;
            break;
        }
    }
}



void ThreadPool::setMode(PoolMode mode)
{
    mode_ = mode;
}

void ThreadPool::setMaxThread(size_t max)
{
    maxThread_ = max;
}
