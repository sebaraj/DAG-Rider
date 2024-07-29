#ifndef BLOCK_BUILDER_HPP
#define BLOCK_BUILDER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <vector>

#include "block.hpp"
#include "broadcaster.hpp"
#include "committee.hpp"

class BlockBuilder : public std::enable_shared_from_this<BlockBuilder> {
public:
    static void spawn(boost::asio::io_context& io_context, std::shared_ptr<Committee> committee);

    BlockBuilder(boost::asio::io_context& io_context, std::shared_ptr<Committee> committee);
    net::awaitable<void> run();
    net::awaitable<void> handle_transaction(const Transaction& transaction);

private:
    boost::asio::io_context& io_context_;
    std::shared_ptr<Committee> committee_;
    std::queue<Transaction> transaction_queue_;
    std::unique_ptr<Broadcaster> network_;
};

#endif  // BLOCK_BUILDER_HPP
