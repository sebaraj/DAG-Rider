#ifndef BLOCK_BUILDER_HPP
#define BLOCK_BUILDER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "block.hpp"
#include "broadcaster.hpp"
#include "committee.hpp"

class BlockBuilder {
public:
    BlockBuilder(boost::asio::io_context& io_context, boost::asio::steady_timer::duration interval,
                 const Committee& committee);

    void spawn();

private:
    void run(boost::asio::yield_context yield);
    void processTransaction(const Transaction& transaction);

    Committee committee_;
    std::queue<Transaction> current_transactions_;
    Broadcaster network_;
    boost::asio::io_context& io_context_;
    boost::asio::steady_timer timer_;
    boost::asio::steady_timer::duration interval_;
    static constexpr size_t BATCH_SIZE = 10;
};

#endif  // BLOCK_BUILDER_HPP
