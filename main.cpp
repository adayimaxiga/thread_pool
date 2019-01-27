#include <iostream>
#include "lib_thread/thread_pool.h"
#include "lib_thread/make_unique.h"


namespace thread_lib_xjturm {
namespace {
    class mytask {

        int number_count=0; GUARDED_BY(mutex_);         //这样保护共享资源

    public:
        void do_something(int tasknumber) {
            //Mutex::Locker locker(&mutex_);
            std::cout<<"XJTU robomaster is best. "<<"num: "<< tasknumber<<std::endl;
            number_count ++;
            std::cout<<"count is "<<number_count<<std::endl;
        }
        Mutex mutex_;
    };
}
}
int task_count=0;
int main() {

    thread_lib_xjturm::ThreadPool pool(5);         //开线程池
    thread_lib_xjturm::mytask mytask;



    for(int i=0;i<10;i++) {
        task_count++;
        auto task = thread_lib_xjturm::make_unique<thread_lib_xjturm::Task>();
        task->SetWorkItem([&mytask]() { mytask.do_something(task_count); });
        pool.Schedule(std::move(task));
    }

    return 0;
}