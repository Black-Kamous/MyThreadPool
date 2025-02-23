#include "../ThreadPool.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>

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

#define FSIZE 4096*4
int writingFile(unsigned int start){
    // write 4KB = 4k chars
    unsigned char s[FSIZE];
    s[0] = (unsigned char)start;
    for(int i=0;i<FSIZE-1;++i){
        s[i+1] = (unsigned char)i;
    }
    std::string name = "/root/MyThreadPool/tests/output/wrote-"+std::to_string(start);
    //std::cout << "Writing " << name << std::endl;
    int fd = open(name.c_str(), O_WRONLY|O_CREAT);
    int res = write(fd, s, FSIZE);
    close(fd);
    return res;
}

int main() {
    ThreadPool tp;
    tp.start(20);
    // tp.setMode(PoolMode::CACHED);
    // tp.setMaxThread(20);
    std::string command = "rm -rf /root/MyThreadPool/tests/output";
    std::string command1 = "mkdir -p /root/MyThreadPool/tests/output";
    int result = std::system(command.c_str());
    result = std::system(command1.c_str());
    std::cout << result << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    auto start = std::chrono::high_resolution_clock::now();
#define TIMES 65536
    for(int i=0;i<TIMES;++i){
        tp.submitTask(writingFile, i);
    }

    while(!tp.taskEmpty()){}
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end-start;
    std::cout << "Multi-Thread Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration) << std::endl;
    

    
    result = std::system(command.c_str());
    result = std::system(command1.c_str());
    std::cout << result << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    start = std::chrono::high_resolution_clock::now();
    for(int i=0;i<TIMES;++i){
        writingFile(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end-start;
    std::cout << "No-Thread Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration) << std::endl;

    return 0;
}