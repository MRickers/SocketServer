#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <server/thread_pool/queue.h>

namespace socket_server {
    using Job = std::function<void()>;

    class ThreadPool {
    private:
        MutexQueue<Job> job_q_;
        std::vector<std::thread> threads_;
        size_t thread_count_;
        size_t threads_init_;
        size_t threads_working_;
        bool running_;

        void worker();
    public:
        ThreadPool(size_t thread_count=std::thread::hardware_concurrency());
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ~ThreadPool();

        void Push(const Job& job);
        size_t Count() const;
        bool Running() const;
        void Terminate();


    };
}