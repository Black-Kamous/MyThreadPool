#include "../ThreadPool.hh"

int run() {
    auto id = std::this_thread::get_id();
    std::cout << "^ Executing task on " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "@ Task over on " << id << std::endl;
    return 0;
}

unsigned long long run1(int i, int j){
    auto id = std::this_thread::get_id();
    std::cout << "^ Executing task on " << id << std::endl;
    
    unsigned long long sum=0;
    for(int p=i;p<=j;++p){
        sum += p;
    }

    std::cout << "@ Task over on " << id << std::endl;
    return sum;
}

int main() {
    ThreadPool tp;
    tp.start(2);
    tp.setMode(PoolMode::CACHED);
    tp.setMaxThread(4);

    auto r1 = tp.submitTask(run);
    auto r2 = tp.submitTask(run1, 1, 100000000);
    auto r3 = tp.submitTask(run1, 1, 3);
    auto r4 = tp.submitTask(run);
    auto r5 = tp.submitTask(run1, 10, 20);
    auto r6 = tp.submitTask(run);

    std::cout << "Submit over." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //tp.stop();

    try{
        int v1 = r1.get();
        std::cout << "r1 " << v1 << std::endl;
        auto v2 = r2.get();
        std::cout << "r2 " << v2 << std::endl;
        int v3 = r3.get();
        std::cout << "r3 " << v3 << std::endl;
        int v4 = r4.get();
        std::cout << "r4 " << v4 << std::endl;
        int v5 = r5.get();
        std::cout << "r5 " << v5 << std::endl;
        int v6 = r6.get();
        std::cout << "r6 " << v6 << std::endl;
    }catch(std::exception& e){
        std::cout << "Exception: " << e.what() << std::endl;
    }

    //std::this_thread::sleep_for(std::chrono::seconds(2));
    getchar();

    return 0;
}