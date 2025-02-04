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

#include "Thread.hh"
#include "Task.hh"

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
    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool operator=(const ThreadPool&) = delete;

    void start(const int = 4);

    Result submitTask(std::shared_ptr<Task>);

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
    std::queue<std::shared_ptr<Task>> taskList_;
    std::atomic_uint currTaskNum_;

    std::condition_variable notEmpty_;
    std::condition_variable notFull_;

    std::mutex poolMutex_;
    std::mutex taskListMutex_;

    std::atomic_bool stopAll_;
    std::condition_variable canStop_;
};
