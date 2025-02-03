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

    void start(const int maxThread = 4);

    Result submitTask(std::shared_ptr<Task>);

    void stop();

    PoolMode mode_;
    
private:
    void workThreadFunc(int);

    size_t maxThread_;
    size_t maxTask_;

    std::unordered_map<int, std::unique_ptr<Thread>> pool_;
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
