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
 * Task类
***********/

class Task {
public:
    Task() {};
    virtual ~Task() = default;
    virtual void execute() = 0;
};