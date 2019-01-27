#include <iostream>
#include "lib_thread/thread_pool.h"
#include "lib_thread/make_unique.h"


namespace thread_lib_xjturm {
namespace {
    class mytask {
    private:
        int number_count=0; GUARDED_BY(mutex_);         //这样保护共享资源
    public:
        void do_something(int number) {

            {
                Mutex::Locker locker(&mutex_);              //共享资源这样上锁,离开大括号范围locker自动释放解锁
                std::cout<<"XJTU robomaster is best. "<<"I am task: "<< number<<std::endl;
            }

            number_count ++;

            {
                Mutex::Locker locker(&mutex_);              //共享资源这样上锁，其他线程阻塞,线程结束则自动撤销
                std::cout<<"count is "<<number_count<<std::endl;
            }
        }

        void how_to_wait_mutex()
        {
            MutexLocker locker(&mutex_);
            locker.Await([this]() REQUIRES(mutex_){
                if(number_count>=10)
                    return 1;
                else
                    return 0;});
        }
        Mutex mutex_;
    };
}
}

int main() {
    int count=0;
    thread_lib_xjturm::ThreadPool pool(5);         //开线程池，线程数目固定为5
    thread_lib_xjturm::mytask mytask;

    for(int i=0;i<10;i++) {
        count++;
        auto task = thread_lib_xjturm::make_unique<thread_lib_xjturm::Task>();
        task->SetWorkItem([&mytask,count]() { mytask.do_something(count); });
        pool.Schedule(std::move(task));
    }
    mytask.how_to_wait_mutex();
    std::cout<<"EveryTask is End";
    return 0;
}