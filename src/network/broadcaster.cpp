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
    } catch (const boost::system::system_error &e) {
      LogNetworkError(NetworkError::FailedToConnect, address, 0);
      co_return;
    }
  }

  auto &socket = it->second;
  try {
    co_await net::async_write(*socket, net::buffer(data), net::use_awaitable);
  } catch (const boost::system::system_error &e) {
    LogNetworkError(NetworkError::FailedToSendMessage, address, 0);
  }
}

net::awaitable<void> Broadcaster::run() {
  // Run the I/O context
  co_await net::this_coro::executor;
}
