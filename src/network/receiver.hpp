#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = net::ip::tcp;

using Writer = tcp::socket;

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    net::awaitable<void> dispatch(Writer &writer, const std::string &message) {
        std::cout << "Received message: " << message << std::endl;
        std::string response = "Acknowledged: " + message + "\n";
        co_await net::async_write(writer, net::buffer(response), net::use_awaitable);
        co_return;
    };
};

template <typename Handler>
class Receiver {
public:
    Receiver(const std::string &address, unsigned short port, std::shared_ptr<Handler> handler);
    void start();
    net::awaitable<void> run();
    net::awaitable<void> handle_connection(tcp::socket socket);
    net::io_context &get_executor() { return io_context_; }

private:
    std::shared_ptr<Handler> handler_;
    tcp::endpoint endpoint_;
    net::io_context io_context_;
};

template <typename Handler>
Receiver<Handler>::Receiver(const std::string &address, unsigned short port, std::shared_ptr<Handler> handler) : handler_(handler), endpoint_(net::ip::make_address(address), port), io_context_() {}

template <typename Handler>
void Receiver<Handler>::start() {
    net::co_spawn(io_context_, run(), net::detached);
    io_context_.run();
}

template <typename Handler>
net::awaitable<void> Receiver<Handler>::run() {
    tcp::acceptor acceptor(io_context_, endpoint_);
    std::cout << "Listening on " << endpoint_.address().to_string() << ":" << endpoint_.port() << std::endl;
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(net::use_awaitable);
        net::co_spawn(io_context_, handle_connection(std::move(socket)), net::detached);
    }
}

template <typename Handler>
net::awaitable<void> Receiver<Handler>::handle_connection(tcp::socket socket) {
    std::string data;

    for (;;) {
        try {
            std::cout << "Reading from socket..." << std::endl;

            auto bytes_transferred = co_await net::async_read_until(socket, net::dynamic_buffer(data), "\n", net::use_awaitable);

            std::cout << "Bytes read from socket: " << bytes_transferred << std::endl;

            std::string message;
            if (!data.empty()) {
                // Extract the string up to the delimiter
                message = data.substr(0, bytes_transferred);
                data.erase(0, bytes_transferred);
            }

            co_await handler_->dispatch(socket, message);

        } catch (const std::exception &e) {
            std::cerr << "Connection error: " << e.what() << std::endl;
            break;
        }
    }
}
#endif  // RECEIVER_HPP
