#include "block_builder.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

#include "block.hpp"
#include "broadcaster.hpp"
#include "committee.hpp"

const size_t BATCH_SIZE = 4;

BlockBuilder::BlockBuilder(boost::asio::io_context& io_context,
                           std::shared_ptr<Committee> committee)
    : io_context_(io_context), committee_(committee), network_(std::make_unique<Broadcaster>()) {}

void BlockBuilder::spawn(boost::asio::io_context& io_context,
                         std::shared_ptr<Committee> committee) {
    auto builder = std::make_shared<BlockBuilder>(io_context, committee);
    net::co_spawn(io_context, builder->run(), net::detached);
}

net::awaitable<void> BlockBuilder::run() {
    while (true) {
        Transaction transaction;
        co_await handle_transaction(transaction);
    }
}

net::awaitable<void> BlockBuilder::handle_transaction(const Transaction& transaction) {
    std::cout << "BlockBuilder: Received transaction." << std::endl;

    // Add the transaction to the queue
    transaction_queue_.push(transaction);

    // If the queue has reached batch size, create a block
    if (transaction_queue_.size() >= BATCH_SIZE) {
        std::vector<Transaction> transactions;
        while (!transaction_queue_.empty()) {
            transactions.push_back(transaction_queue_.front());
            transaction_queue_.pop();
        }

        Block block(transactions);
        nlohmann::json block_json = block;  // Assuming to_json is defined for Block
        std::string serialized_block = block_json.dump();

        // Broadcast the block to the committee members
        for (const auto& endpoint : committee_->get_block_receiver_addresses()) {
            std::string address = endpoint.address().to_string();
            unsigned short port = endpoint.port();
            co_await network_->send(address, port, serialized_block);
        }

        std::cout << "BlockBuilder: Block created and broadcasted." << std::endl;
    }

    co_return;
}
