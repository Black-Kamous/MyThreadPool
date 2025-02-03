#include "ThreadPool.hh"

ThreadPool::ThreadPool()
    : mode_(PoolMode::FIXED),
    maxThread_(0),
    maxTask_(1024),
    stopAll_(false)
{}

void ThreadPool::stop(){
    stopAll_ = true;
    notEmpty_.notify_all();
    std::unique_lock<std::mutex> lock(taskListMutex_);
    canStop_.wait(lock, [&](){ return (taskList_.empty() && pool_.empty()); });
    std::cout << "Pool safely quited" << std::endl;
}

ThreadPool::~ThreadPool()
{
    if(!stopAll_)
        this->stop();
}

void ThreadPool::start(const int maxThread)
{
    maxThread_ = maxThread;
    std::function<void(int)> f = std::bind(&ThreadPool::workThreadFunc, this, std::placeholders::_1);

    for(int i=0;i<maxThread;++i) {
        pool_.emplace(i, std::make_unique<Thread>(f, i));
    }

    for(int i=0;i<maxThread;++i) {
        std::cout << "Started a thread" << std::endl;
        pool_[i]->start();
        idleThreadNum_++;
    }

    notFull_.notify_all();
}

void ThreadPool::workThreadFunc(int id)
{
    std::shared_ptr<Task> spTask;
    while(true){
        {
        std::unique_lock<std::mutex> lock(taskListMutex_);
        notEmpty_.wait(lock, [&]() {return (!taskList_.empty() || stopAll_);});
        if(!taskList_.empty()){
            std::cout << "# Get a task on " << std::this_thread::get_id() << std::endl;
            spTask = taskList_.front();
            taskList_.pop();
            currTaskNum_--;
        }else{
            spTask = nullptr;
        }

        if(!taskList_.empty())
            notEmpty_.notify_all();

        notFull_.notify_all();
        }

        if(spTask != nullptr) {
            idleThreadNum_--;
            spTask->execute();
            idleThreadNum_++;
        }

        if(stopAll_) {
            idleThreadNum_--;
            pool_.erase(id);
            canStop_.notify_all();
            std::cout << "- Thread quit on " << std::this_thread::get_id() << std::endl;
            break;
        }
    }
}

int ThreadPool::submitTask(std::shared_ptr<Task> spTask)
{
    std::unique_lock<std::mutex> lock(taskListMutex_);

    if(!notFull_.wait_for(lock, std::chrono::seconds(1), 
                        [&]() { return taskList_.size() < maxTask_; })){
        std::cout << "Task list full" << std::endl;
        return -1;
    }
    std::cout << "+ Submitted a Task" << std::endl;

    taskList_.emplace(spTask);
    currTaskNum_++;

    notEmpty_.notify_all();

    return 0;
}
