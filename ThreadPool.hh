#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <iostream>
#include <memory>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <type_traits>


#include "Thread.hh"

const int RECOLLECT_TIME_SEC = 60;

/**********
 * ThreadPool类
***********/

enum PoolMode {
    CACHED,
    FIXED
};

class ThreadPool {
public:
    using Task = std::function<void()>;
    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool operator=(const ThreadPool&) = delete;

    void start(const int = 4);
    bool taskEmpty();

    template<typename Func, typename... Args>
    auto submitTask(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>
    {
        std::unique_lock<std::mutex> lock(taskListMutex_);
        using Rtype = decltype(func(args...));
        auto f0 = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
        auto spTask = std::make_shared<std::packaged_task<Rtype()>>(
            f0);
        auto future = spTask->get_future();

        if(!notFull_.wait_for(lock, std::chrono::seconds(1), 
                            [&]() { return taskList_.size() < maxTask_; })){
            if(verbose) std::cout << "Task list full" << std::endl;
            auto failTask = std::make_shared<std::packaged_task<Rtype()>>([](){return Rtype();});
            auto failFuture = failTask->get_future();
            (*failTask)();
            return failFuture;
        }
        if(verbose) std::cout << "+ Submitted a Task" << std::endl;

        taskList_.emplace([spTask]()->void { (*spTask)(); });
        currTaskNum_++;

        notEmpty_.notify_all();

        if(mode_ == PoolMode::CACHED
            &&
            currTaskNum_ > idleThreadNum_
            &&
            curThread_ < maxThread_){
            static std::atomic_int offset = 0;
            std::function<void(int)> f = std::bind(&ThreadPool::workThreadFunc, this, std::placeholders::_1);

            pool_.emplace(fixedThread_+offset, std::make_unique<Thread>(f, fixedThread_+offset));
            if(verbose) std::cout << "= Started a cached thread" << std::endl;
            pool_[fixedThread_+offset]->start();
            curThread_++;
            idleThreadNum_++;
        }

        return future;
    }

    void stop();

    void setMode(PoolMode);

    void setMaxThread(size_t);
    
private:
    void workThreadFunc(int);

    PoolMode mode_;
    size_t fixedThread_;
    size_t maxThread_;
    size_t maxTask_;

    std::unordered_map<int, std::unique_ptr<Thread>> pool_;
    std::atomic_uint curThread_;
    std::atomic_uint idleThreadNum_;

    std::queue<Task> taskList_;
    std::atomic_uint currTaskNum_;

    std::condition_variable notEmpty_;
    std::condition_variable notFull_;

    std::mutex poolMutex_;
    std::mutex taskListMutex_;

    std::atomic_bool stopAll_;
    std::condition_variable canStop_;

    bool verbose = false;
};
