#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <typename T>
class Receiver {
public:
    std::optional<T> receive() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&] { return !queue_.empty(); });
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    void send(T item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
        }
        cv_.notify_one();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

template <typename T>
class Sender {
public:
    Sender(Receiver<T>& receiver) : receiver_(receiver) {}

    void send(T item) { receiver_.send(std::move(item)); }

private:
    Receiver<T>& receiver_;
};

#endif  // COMMUNICATION_HPP
