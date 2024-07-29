#include "coordinator.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

#include "block_builder.hpp"

// Implementation of TxReceiverHandler
TxReceiverHandler::TxReceiverHandler(std::shared_ptr<Committee> committee)
    : committee_(committee) {}

net::awaitable<void> TxReceiverHandler::dispatch(Writer &writer, const std::string &message) {
    try {
        // Handle the received transaction
        std::cout << "TxReceiverHandler: Received transaction message.\n";

        // Deserialize the transaction
        nlohmann::json j = nlohmann::json::parse(message);
        Transaction transaction;
        j.get_to(transaction);  // Assuming a from_json function is defined for Transaction

        // Add the transaction to the queue
        transaction_queue_.push(transaction);

        // If the queue has reached a certain size, create a block
        const size_t BATCH_SIZE = 4;
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
            Broadcaster broadcaster;
            for (const auto &endpoint : committee_->get_block_receiver_addresses()) {
                std::string address = endpoint.address().to_string();
                unsigned short port = endpoint.port();
                co_await broadcaster.send(address, port, serialized_block);
            }

            std::cout << "TxReceiverHandler: Block created and broadcasted.\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Error in TxReceiverHandler::dispatch: " << e.what() << std::endl;
    }

    co_return;
}

// Implementation of BlockReceiverHandler
BlockReceiverHandler::BlockReceiverHandler(std::shared_ptr<Committee> committee)
    : committee_(committee) {}

net::awaitable<void> BlockReceiverHandler::dispatch(Writer &writer, const std::string &message) {
    try {
        // Handle the received block
        std::cout << "BlockReceiverHandler: Received block message.\n";

        // Deserialize the block message
        nlohmann::json j = nlohmann::json::parse(message);
        Block block;
        j.get_to(block);  // Assuming a from_json function is defined for Block

        // Process the block as needed
        std::cout << "BlockReceiverHandler: Block has " << block.transactions_.size()
                  << " transactions.\n";
    } catch (const std::exception &e) {
        std::cerr << "Error in BlockReceiverHandler::dispatch: " << e.what() << std::endl;
    }

    co_return;
}

// Implementation of TransactionCoordinator
void TransactionCoordinator::spawn(std::shared_ptr<Committee> committee,
                                   const std::string &tx_address, unsigned short tx_port,
                                   const std::string &block_address, unsigned short block_port) {
    auto tx_handler = std::make_shared<TxReceiverHandler>(committee);
    auto block_handler = std::make_shared<BlockReceiverHandler>(committee);

    auto tx_receiver
        = std::make_shared<Receiver<TxReceiverHandler>>(tx_address, tx_port, tx_handler);
    auto block_receiver = std::make_shared<Receiver<BlockReceiverHandler>>(
        block_address, block_port, block_handler);

    std::thread([tx_receiver]() { tx_receiver->start(); }).detach();

    std::thread([block_receiver]() { block_receiver->start(); }).detach();
    // tx_receiver->start();
    // block_receiver->start();
}
