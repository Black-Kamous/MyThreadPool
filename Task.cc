#include "Task.hh"

Result::Result(std::shared_ptr<Task> task, bool isValid)
    :sem_(0),
    task_(task),
    isValid_(isValid)
{
    task_->setResult(this);
}

Result::~Result()
{
    isValid_ = false;
    
    std::cout << "result deleted" << std::endl;
}

void Result::setVal(std::any any)
{
    data_ = any;
    std::cout << "Setting any" << std::endl;
    sem_.release();
}

std::any Result::getVal()
{
    if(!isValid_){
        return "";
    }
    std::cout << "Try getting any" << std::endl;
    sem_.acquire();
    return data_;
}

Task::Task()
    : res_{nullptr}
{
}

void Task::execute()
{
    res_->setVal(run());
}

void Task::setResult(Result *res)
{
    res_ = res;
}
