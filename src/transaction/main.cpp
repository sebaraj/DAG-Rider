#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

#include "block.hpp"
#include "block_builder.hpp"
#include "broadcaster.hpp"
#include "committee.hpp"
#include "coordinator.hpp"
#include "validator.hpp"

std::vector<unsigned char> string_to_vector(const std::string& str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}

void simulate_transactions(boost::asio::io_context& io_context,
                           std::shared_ptr<BlockBuilder> block_builder) {
    std::vector<Transaction> transactions;
    for (int i = 0; i < 15; ++i) {
        transactions.emplace_back(string_to_vector("Transaction_" + std::to_string(i)));
    }

    for (const auto& tx : transactions) {
        boost::asio::co_spawn(io_context, block_builder->handle_transaction(tx),
                              boost::asio::detached);
    }
}

void simulate_block_receiver(boost::asio::io_context& io_context,
                             std::shared_ptr<Committee> committee) {
    std::string block_address = "127.0.0.1";

    auto handler = [](const std::string& data) {
        std::cout << "Received block data: " << data << std::endl;
    };

    std::thread([block_address, handler]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        handler("Simulated block data received");
    }).detach();
}

int main() {
    try {
        boost::asio::io_context io_context;
        auto committee = std::make_shared<Committee>();

        // Setup and test BlockBuilder
        std::cout << "Testing BlockBuilder..." << std::endl;
        auto block_builder = std::make_shared<BlockBuilder>(io_context, committee);
        simulate_transactions(io_context, block_builder);

        // Setup and test TransactionCoordinator
        std::cout << "Testing TransactionCoordinator..." << std::endl;
        std::string tx_address = "127.0.0.1";
        unsigned short tx_port = 1234;
        std::string block_address = "127.0.0.1";
        unsigned short block_port = 5678;

        TransactionCoordinator::spawn(committee, tx_address, tx_port, block_address, block_port);

        // Simulate block receiver
        std::cout << "Simulating Block Receiver..." << std::endl;
        simulate_block_receiver(io_context, committee);

        // Run the io_context to handle asynchronous operations
        io_context.run();

        std::cout << "All components tested successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
