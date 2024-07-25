#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "block.hpp"
#include "broadcaster.hpp"
#include "committee.hpp"
#include "receiver.hpp"

class Coordinator {
public:
    Coordinator(boost::asio::io_context& io_context, const Committee& committee,
                std::function<void(const Block&)> block_sender);

    void spawn(const Id& node_id);

private:
    class TxReceiverHandler : public MessageHandler {
    public:
        TxReceiverHandler(std::function<void(const Transaction&)> transaction_sender);
        net::awaitable<void> dispatch(Writer& writer, const std::string& message);

    private:
        std::function<void(const Transaction&)> transaction_sender_;
    };

    class BlockReceiverHandler : public MessageHandler {
    public:
        BlockReceiverHandler(std::function<void(const Block&)> block_sender);
        net::awaitable<void> dispatch(Writer& writer, const std::string& message);

    private:
        std::function<void(const Block&)> block_sender_;
    };

    Committee committee_;
    boost::asio::io_context& io_context_;
    std::function<void(const Block&)> block_sender_;
};

#endif  // COORDINATOR_HPP
