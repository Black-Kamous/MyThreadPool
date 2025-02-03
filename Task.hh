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
#include <any>
#include <semaphore>

/**********
 * Task类
***********/

class Task;

class Result {
public:
    Result(std::shared_ptr<Task>, bool = true);
    ~Result();

    void setVal(std::any);
    std::any getVal();

private:
    std::any data_;
    std::binary_semaphore sem_;
    std::shared_ptr<Task> task_;
    std::atomic_bool isValid_;

};

class Task {
public:
    Task();
    void execute();
    virtual ~Task() = default;
    virtual std::any run() = 0;
    void setResult(Result*);

private:
    Result* res_;
};