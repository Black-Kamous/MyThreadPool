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


/**********
 * Thread类
***********/

class Thread {
public:
    using ThreadFunc = std::function<void(int)>;
    Thread(ThreadFunc, int);
    ~Thread();
    void start();
private:
    ThreadFunc f_;
    int tid_;
};