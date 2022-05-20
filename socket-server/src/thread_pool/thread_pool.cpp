#include <server/thread_pool/thread_pool.h>
#include <logging/logging.h>
#include <server/error.h>

namespace socket_server {
    ThreadPool::ThreadPool(size_t thread_count) : 
    job_q_(),
    threads_(), 
    thread_count_(thread_count),
    threads_init_(0),
    threads_working_(0),
    running_(true),
    cond_(),
    mutex_()
     {
        for(size_t i = 0;i<thread_count_;++i) {
            threads_.push_back(std::thread(&ThreadPool::worker, this));
        }
        while(thread_count_ != threads_init_);
    }

    void ThreadPool::worker() {
        ++threads_init_;
        while(running_) {
            workAvailable();

            if(!running_) {
                break;
            }

            if(const auto job = job_q_.Pop(); job.has_value()) {
                const auto& callback = job.value();
                callback();
            }
        }
    }

    void ThreadPool::workAvailable() {
        std::unique_lock<std::mutex> lock(mutex_);

        if(job_q_.Empty()) {
            cond_.wait(lock, [&] {
                return !running_ || !job_q_.Empty();
            });
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

    void ThreadPool::Push(const Job& job) {
        const std::lock_guard<std::mutex> lock(mutex_);

        if(job_q_.Size() >= thread_count_ * 2) {
            lLog(lWarn) << "ThreadPool full";
            throw ServerException("ThreadPool full", -1000);
        }

        job_q_.Push(job);
        cond_.notify_all();
    }

    void ThreadPool::Terminate() {
        running_ = false;
        cond_.notify_all();
    }

    size_t ThreadPool::WorkingThreads() const {
        return threads_working_;
    }

    size_t ThreadPool::Count() const {
        return thread_count_;
    }

    bool ThreadPool::Running() const {
        return running_;
    }
}