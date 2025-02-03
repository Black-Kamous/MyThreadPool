#include "../ThreadPool.hh"

class MyTask : public Task {
public:
    MyTask(){}
    ~MyTask(){};

    std::any run() {
        auto id = std::this_thread::get_id();
        std::cout << "^ Executing task on " << id << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "@ Task over on " << id << std::endl;
        return 0;
    }
};

int main() {
    ThreadPool tp;
    tp.start();

    Result r1 = tp.submitTask(std::make_shared<MyTask>());
    Result r2 = tp.submitTask(std::make_shared<MyTask>());
    Result r3 = tp.submitTask(std::make_shared<MyTask>());
    Result r4 = tp.submitTask(std::make_shared<MyTask>());
    Result r5 = tp.submitTask(std::make_shared<MyTask>());
    Result r6 = tp.submitTask(std::make_shared<MyTask>());

    std::cout << "Submit over." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //tp.stop();

    try{
        int v1 = std::any_cast<int>(r1.getVal());
        std::cout << "r1 " << v1 << std::endl;
        int v2 = std::any_cast<int>(r2.getVal());
        std::cout << "r2 " << v2 << std::endl;
        int v3 = std::any_cast<int>(r3.getVal());
        std::cout << "r3 " << v3 << std::endl;
        int v4 = std::any_cast<int>(r4.getVal());
        std::cout << "r4 " << v4 << std::endl;
        int v5 = std::any_cast<int>(r5.getVal());
        std::cout << "r5 " << v5 << std::endl;
        int v6 = std::any_cast<int>(r6.getVal());
        std::cout << "r6 " << v6 << std::endl;
    }catch(std::exception& e){
        std::cout << "Exception: " << e.what() << std::endl;
    }

    //std::this_thread::sleep_for(std::chrono::seconds(2));
    getchar();

    return 0;
}