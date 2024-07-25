#include "coordinator.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>

Coordinator::Coordinator(boost::asio::io_context& io_context, const Committee& committee,
                         std::function<void(const Block&)> block_sender)
    : committee_(committee), io_context_(io_context), block_sender_(block_sender) {}

void Coordinator::spawn(const Id& node_id) {
    auto tx_address = committee_.get_tx_receiver_address(node_id).address();
    std::cout << "Start listening for transactions on " << tx_address << std::endl;
    auto tx_handler = std::make_shared<TxReceiverHandler>([](const Transaction& transaction) {
        // Handle the transaction (e.g., forward to BlockBuilder)
    });
    auto tx_receiver = std::make_shared<Receiver<TxReceiverHandler>>(
        tx_address, committee_.get_tx_receiver_address(node_id).port(), tx_handler);
    tx_receiver->start();

    auto block_address = committee_.get_block_receiver_address(node_id).address();
    std::cout << "Start listening for blocks on " << block_address << std::endl;
    auto block_handler = std::make_shared<BlockReceiverHandler>(block_sender_);
    auto block_receiver = std::make_shared<Receiver<BlockReceiverHandler>>(
        block_address, committee_.get_block_receiver_address(node_id).port(), block_handler);
    block_receiver->start();
}

Coordinator::TxReceiverHandler::TxReceiverHandler(
    std::function<void(const Transaction&)> transaction_sender)
    : transaction_sender_(transaction_sender) {}

net::awaitable<void> Coordinator::TxReceiverHandler::dispatch(Writer& writer,
                                                              const std::string& message) {
    std::cout << "TxReceiverHandler received transaction to process: " << message << std::endl;
    Transaction transaction;  // Deserialize the transaction object from message
    transaction_sender_(transaction);
    co_return;
}

Coordinator::BlockReceiverHandler::BlockReceiverHandler(
    std::function<void(const Block&)> block_sender)
    : block_sender_(block_sender) {}

net::awaitable<void> Coordinator::BlockReceiverHandler::dispatch(Writer& writer,
                                                                 const std::string& message) {
    std::cout << "BlockReceiverHandler received block to process: " << message << std::endl;
    Block block;  // Deserialize the block object from message
    block_sender_(block);
    co_return;
}
