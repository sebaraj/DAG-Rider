#include "broadcaster.hpp"
#include "error.hpp"

Broadcaster::Broadcaster()
    : rng_(std::random_device()()), io_context_(),
      work_(net::make_work_guard(io_context_)) {}

void Broadcaster::start() {
  net::co_spawn(io_context_, run(), net::detached);
  io_context_.run();
}

net::awaitable<void> Broadcaster::send(const std::string &address,
                                       uint16_t port, const std::string &data) {
  auto [it, inserted] = connections_.emplace(address, nullptr);
  if (inserted) {
    it->second =
        std::make_shared<tcp::socket>(co_await net::this_coro::executor);
    try {
      co_await it->second->async_connect(
          tcp::endpoint(net::ip::make_address(address), port),
          net::use_awaitable);
      std::cout << "Connected to " << address << ":" << port << std::endl;
    } catch (const boost::system::system_error &e) {
      LogNetworkError(NetworkError::FailedToConnect, address, 0);
      co_return;
    }
  }

  // Print out all the details in connections_
  std::cout << "Current connections:" << std::endl;
  for (const auto &conn : connections_) {
    std::cout << "Address: " << conn.first
              << ", Socket: " << (conn.second ? "Connected" : "Not connected")
              << std::endl;
  }

  auto &socket = it->second;
  try {
    std::string message = data; // + "\n"; // Ensure message ends with newline
    co_await net::async_write(*socket, net::buffer(message),
                              net::use_awaitable);
    std::cout << "Sent " << message << " to " << address << ":" << port
              << std::endl;
  } catch (const boost::system::system_error &e) {
    LogNetworkError(NetworkError::FailedToSendMessage, address, 0);
  }
}

net::awaitable<void> Broadcaster::run() { co_return; }
