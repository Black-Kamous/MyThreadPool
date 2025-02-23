#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cassert>

#define FSIZE 4096*4
int checkFile(unsigned int start){
    // write 4KB = 4k chars
    unsigned char s[FSIZE];
    std::string name = "/root/MyThreadPool/tests/output/wrote-"+std::to_string(start);
    std::cout << "Reading " << name << std::endl;
    int fd = open(name.c_str(), O_RDONLY);
    int res = read(fd, s, FSIZE);
    assert(res == FSIZE);
    close(fd);
    assert(s[0] == (unsigned char)start);
    for(int i=0;i<FSIZE-1;++i){
        assert(s[i+1] == (unsigned char)i);
    }
    return res;
}

#define TIMES 65536
int main(){
    for(int i=0;i<TIMES;++i){
        checkFile(i);
    }
    return 0;
}

