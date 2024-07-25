#include "blockbuilder.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>

BlockBuilder::BlockBuilder(boost::asio::io_context& io_context,
                           boost::asio::steady_timer::duration interval, const Committee& committee)
    : io_context_(io_context),
      interval_(interval),
      timer_(io_context),
      committee_(committee),
      network_(Broadcaster()) {}

void BlockBuilder::spawn() {
    boost::asio::spawn(io_context_, std::bind(&BlockBuilder::run, this, std::placeholders::_1));
}

void BlockBuilder::run(boost::asio::yield_context yield) {
    while (true) {
        // Simulate async transaction receiving
        Transaction transaction;
        timer_.expires_after(interval_);
        timer_.async_wait(yield);

        processTransaction(transaction);

        if (current_transactions_.size() >= BATCH_SIZE) {
            std::cout << "BlockBuilder has enough transactions to make a block. Broadcasting it."
                      << std::endl;

            Block block;
            while (!current_transactions_.empty()) {
                block.addTransaction(current_transactions_.front());
                current_transactions_.pop();
            }

            std::stringstream ss;
            boost::archive::text_oarchive oa(ss);
            oa << block;

            std::string serialized_block = ss.str();
            std::vector<std::future<void>> handlers
                = network_.broadcast(committee_.getBlockReceiverAddresses(), serialized_block);

            for (auto& handler : handlers) {
                try {
                    handler.get();  // Await the handler
                } catch (const std::exception& e) {
                    std::cerr << "Broadcast of the block was not successful: " << e.what()
                              << std::endl;
                }
            }
        }
    }
}

void BlockBuilder::processTransaction(const Transaction& transaction) {
    std::cout << "BlockBuilder received transaction" << std::endl;
    current_transactions_.push(transaction);
}
