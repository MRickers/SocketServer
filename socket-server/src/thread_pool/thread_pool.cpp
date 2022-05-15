#include <server/thread_pool/thread_pool.h>

namespace socket_server {
    ThreadPool::ThreadPool(size_t thread_count) : 
    job_q_(),
    threads_(), 
    thread_count_(thread_count),
    threads_init_(0),
    threads_working_(0),
    running_(true)
     {
        for(size_t i = 0;i<thread_count_;++i) {
            threads_.push_back(std::thread(&ThreadPool::worker, this));
        }
        while(thread_count_ != threads_init_);
    }

    void ThreadPool::worker() {
        ++threads_init_;
        while(running_) {

        }
    }

    ThreadPool::~ThreadPool() {
        Terminate();
        
        for(auto& thread : threads_) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    }

    void ThreadPool::Terminate() {
        running_ = false;
    }

    size_t ThreadPool::Count() const {
        return threads_working_;
    }

    bool ThreadPool::Running() const {
        return running_;
    }
}