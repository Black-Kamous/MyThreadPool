#include "../ThreadPool.hh"

class MyTask : public Task {
public:
    MyTask(){}
    ~MyTask(){};

    void execute() {
        auto id = std::this_thread::get_id();
        std::cout << "^ Executing task on " << id << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "@ Task over on " << id << std::endl;
    }
};

int main() {
    char c;
    ThreadPool tp;
    tp.start();

    tp.submitTask(std::make_shared<MyTask>());
    tp.submitTask(std::make_shared<MyTask>());
    tp.submitTask(std::make_shared<MyTask>());
    tp.submitTask(std::make_shared<MyTask>());
    tp.submitTask(std::make_shared<MyTask>());
    tp.submitTask(std::make_shared<MyTask>());

    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::cout << "Over." << std::endl;
    tp.stop();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}