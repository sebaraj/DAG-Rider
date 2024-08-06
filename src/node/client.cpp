#include "client.hpp"

#include <array>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

using tcp = boost::asio::ip::tcp;

Client::Client(boost::asio::io_context& io_context, const std::string& address, unsigned short port)
    : io_context_(io_context),
      endpoint_(boost::asio::ip::make_address(address), port),
      socket_(io_context) {}

void Client::send() {
    const std::size_t TRANSACTION_COUNT = 40;
    const std::size_t TX_SIZE = 64;

    transaction_count_ = TRANSACTION_COUNT;
    tx_size_ = TX_SIZE;

    socket_.async_connect(endpoint_, [this](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "Connected to " << endpoint_ << std::endl;
            do_send(0);
        } else {
            std::cerr << "Failed to connect: " << ec.message() << std::endl;
        }
    });
}

void Client::do_send(std::size_t count) {
    if (count >= transaction_count_) {
        std::cout << "All transactions sent" << std::endl;
        return;
    }

    std::array<uint8_t, 64> tx;
    tx[0] = 0;                                     // Sample txs start with 0.
    *reinterpret_cast<uint64_t*>(&tx[1]) = count;  // This counter identifies the tx.

    boost::asio::async_write(socket_, boost::asio::buffer(tx),
                             [this, count](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                     std::cout << "Sent transaction " << count << std::endl;
                                     do_send(count + 1);
                                 } else {
                                     std::cerr << "Failed to send transaction: " << ec.message()
                                               << std::endl;
                                 }
                             });
}
