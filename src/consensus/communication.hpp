#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>

template <typename T>
class Receiver {
public:
    std::optional<T> receive() {
        // std::cout << "Locking mutex in receive() p1" << std::endl;
        std::unique_lock<std::mutex> lock(mutex_);
        // std::cout << "Locking mutex in receive() p2" << std::endl;
        {
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            // std::cout << "Locking mutex in receive()" << std::endl;
        }
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

    void printContents() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "mutex still exists..." << std::endl;
    }
    std::mutex cout_mutex;

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
