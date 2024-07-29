#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <string>

#include "block.hpp"
#include "committee.hpp"
#include "receiver.hpp"

class TxReceiverHandler : public MessageHandler,
                          public std::enable_shared_from_this<TxReceiverHandler> {
public:
    TxReceiverHandler(std::shared_ptr<Committee> committee);
    net::awaitable<void> dispatch(Writer &writer, const std::string &message);

private:
    std::shared_ptr<Committee> committee_;
    std::queue<Transaction> transaction_queue_;
};

class BlockReceiverHandler : public MessageHandler,
                             public std::enable_shared_from_this<BlockReceiverHandler> {
public:
    BlockReceiverHandler(std::shared_ptr<Committee> committee);
    net::awaitable<void> dispatch(Writer &writer, const std::string &message);

private:
    std::shared_ptr<Committee> committee_;
};

class TransactionCoordinator {
public:
    static void spawn(std::shared_ptr<Committee> committee, const std::string &tx_address,
                      unsigned short tx_port, const std::string &block_address,
                      unsigned short block_port);
};

#endif  // COORDINATOR_HPP
