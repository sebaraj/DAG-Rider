#ifndef BROADCASTER_HPP
#define BROADCASTER_HPP

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>

#include "error.hpp"

namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = net::ip::tcp;

class Broadcaster {
public:
    Broadcaster();
    void start();
    net::awaitable<void> send(const std::string &address, uint16_t port, const std::string &data);
    net::io_context &get_io_context() { return io_context_; }

private:
    net::awaitable<void> run();

    std::unordered_map<std::string, std::shared_ptr<tcp::socket>> connections_;
    std::mt19937 rng_;
    net::io_context io_context_;
    net::executor_work_guard<net::io_context::executor_type> work_;
};

#endif  // BROADCASTER_HPP
