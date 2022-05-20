#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <server/thread_pool/queue.h>

namespace socket_server {
    using Job = std::function<void()>;

    class ThreadPool {
    private:
        MutexQueue<Job> job_q_;
        std::vector<std::thread> threads_;
        std::atomic<size_t> thread_count_{0};
        std::atomic<size_t> threads_init_{0};
        std::atomic<size_t> threads_working_{0};
        std::atomic<bool> running_{false};
        std::condition_variable cond_;
        mutable std::mutex mutex_;

        void worker();
        void workAvailable();
    public:
        ThreadPool(size_t thread_count=std::thread::hardware_concurrency());
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ~ThreadPool();

        void Push(const Job& job);
        size_t Count() const;
        size_t WorkingThreads() const;
        bool Running() const;
        void Terminate();
    };
}