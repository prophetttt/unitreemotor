#ifndef CORETHREAD
#define CORETHREAD
#include <string>
#include <functional>
#include <thread>
#include <queue>
#define RUN_IN_THREAD

//motorControl thread abstraction
template<typename T>
class CoreThread
{
private:
    //store thread instance
    static CoreThread * coreThreadNum[100];
    T motordata;
    CoreThread(unsigned int id);
    std::queue<T> buffer;
    std::mutex mtx;
    std::condition_variable cv_consumer;
    std::condition_variable cv_producter;
    RUN_IN_THREAD void run();

public:
CoreThread(const CoreThread&) = delete;
CoreThread& operator=(const CoreThread&) = delete;
CoreThread(CoreThread&&) = delete;
CoreThread& operator=(CoreThread&&) = delete;
static std::unique_ptr<CoreThread> create (const unsigned int& id);



}
#endif