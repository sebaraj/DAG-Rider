#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "broadcaster.hpp"
#include "receiver.hpp"

class SimpleMessageHandler : public MessageHandler {
public:
    net::awaitable<void> dispatch(Writer &writer, const std::string &message) {
        std::cout << "Received message: " << message << std::endl;
        std::string response = "Acknowledged: " + message + "\n";
        co_await net::async_write(writer, net::buffer(response), net::use_awaitable);
        co_return;
    }
};

int main() {
    auto handler = std::make_shared<MessageHandler>();
    Receiver<MessageHandler> receiver("127.0.0.1", 8080, handler);
    Broadcaster broadcaster;

    // Start the receiver in a separate thread
    std::thread receiver_thread([&receiver]() { receiver.start(); });

    // Wait for the server to start
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Test send function
    net::co_spawn(
        receiver.get_executor(),
        [&broadcaster]() -> net::awaitable<void> {
            std::cout << "Testing send function" << std::endl;
            co_await broadcaster.send("127.0.0.1", 8080, "Hello, world!\n");
            std::cout << "Send function test completed" << std::endl;
        },
        net::detached);

    // Wait before testing the next function
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Test broadcasting to multiple addresses
    net::co_spawn(
        receiver.get_executor(),
        [&broadcaster]() -> net::awaitable<void> {
            std::cout << "Testing broadcast function" << std::endl;
            std::vector<std::string> addresses = {"127.0.0.1"};
            for (const auto &address : addresses) {
                std::cout << "Broadcasting to: " << address << std::endl;
                co_await broadcaster.send(address, 8080, "Broadcast message\n");
            }
            std::cout << "Broadcast function test completed" << std::endl;
            co_return;
        },
        net::detached);

    // Wait before testing the next function
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Test lucky broadcast
    net::co_spawn(
        receiver.get_executor(),
        [&broadcaster]() -> net::awaitable<void> {
            std::cout << "Testing lucky broadcast function" << std::endl;
            std::vector<std::string> addresses = {"127.0.0.1"};
            std::shuffle(addresses.begin(), addresses.end(), std::mt19937{std::random_device{}()});
            addresses.resize(std::min<size_t>(addresses.size(), 1));
            for (const auto &address : addresses) {
                std::cout << "Lucky broadcasting to: " << address << std::endl;
                co_await broadcaster.send(address, 8080, "Lucky broadcast message\n");
            }
            std::cout << "Lucky broadcast function test completed" << std::endl;
            co_return;
        },
        net::detached);

    broadcaster.start();

    // Join the receiver thread before exiting
    receiver_thread.join();

    return 0;
}
