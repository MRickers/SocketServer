#include <deque>
#include <mutex>
#include <optional>

namespace socket_server {
    template<typename T>
    class MutexQueue {
    private:
        std::deque<T> queue_;
        mutable std::mutex mtx_;
    public:
        MutexQueue() = default;
        MutexQueue(const MutexQueue& other) : queue_(other.queue_.begin(), other.queue_.end()), mtx_() {
        }

        MutexQueue& operator=(const MutexQueue& other) {
            if(this != &other) {
                queue_ = other.queue_;
            } 
            return *this;
        }

        void Push(const T& data) {
            const std::lock_guard<std::mutex> lock(mtx_);

            queue_.emplace_back(data);
        }

        std::optional<T> Pop() {
            const std::lock_guard<std::mutex> lock(mtx_);

            if(!queue_.empty()) {
                const auto item = queue_.front();
                queue_.pop_front();
                return item;
            }
            return {};
        }

        size_t Size() const {
            const std::lock_guard<std::mutex> lock(mtx_);

            return queue_.size();
        }

        void Clear() {
            const std::lock_guard<std::mutex> lock(mtx_);

            for([[maybe_unused]]const auto& item : queue_) {
                queue_.pop_front();
            }
        }
    };
}