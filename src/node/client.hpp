#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <boost/asio.hpp>
#include <string>

class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& address, unsigned short port);
    void send();

private:
    void do_send(std::size_t count);

    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::socket socket_;
    std::size_t transaction_count_;
    std::size_t tx_size_;
};

#endif  // CLIENT_HPP
