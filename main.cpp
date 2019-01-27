#include <iostream>
#include "lib_thread/thread_pool.cc"
#include "lib_thread/make_unique.h"

namespace cartographer {
namespace common {
namespace {





int main() {

    ThreadPool pool(3);         //开线程池

    auto  task = common::make_unique<Task>();


    std::cout << "Hello, World!" << std::endl;
    return 0;
}






}
}
}