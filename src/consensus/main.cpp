#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "block.hpp"
#include "committee.hpp"
#include "communication.hpp"
#include "consensus.hpp"
#include "vertex.hpp"

int main() {
    std::cout << "Starting main program..." << std::endl;

    // Create committee and genesis vertices
    std::cout << "Creating committee and genesis vertices..." << std::endl;
    Committee committee;
    std::vector<NodePublicKey> nodeKeys = committee.get_nodes_keys();
    std::vector<Vertex> genesisVertices = Vertex::genesis(nodeKeys);

    // Create receivers and senders for blocks and vertices
    std::cout << "Creating receivers and senders..." << std::endl;
    Receiver<Vertex> vertexReceiver;
    Receiver<Vertex> vertexToBroadcastReceiver;
    Receiver<Vertex> vertexOutputReceiver;
    Receiver<Block> blocksReceiver;

    Sender<Vertex> vertexToBroadcastSender(vertexToBroadcastReceiver);
    Sender<Vertex> vertexOutputSender(vertexOutputReceiver);
    Sender<Block> blockSender(blocksReceiver);
    Sender<Vertex> vertexSender(vertexReceiver);  // new
    // Create and start the consensus algorithm
    std::cout << "Creating and starting the consensus algorithm..." << std::endl;
    NodePublicKey nodeId = nodeKeys[0];
    Consensus::spawn(nodeId, committee, vertexReceiver, vertexToBroadcastSender, vertexOutputSender,
                     blocksReceiver);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "reached and running..." << std::endl;
    // Simulate adding blocks and vertices
    for (int i = 1; i <= 5; ++i) {
        std::cout << "Sending Block " << i << " to the consensus algorithm..." << std::endl;
        Block block;
        block.addTransaction("Transaction " + std::to_string(i));
        blockSender.send(block);

        // Simulate a delay
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Add some vertices to the consensus
    std::cout << "Adding genesis vertices to the consensus..." << std::endl;
    for (const auto& vertex : genesisVertices) {
        // vertexReceiver.printContents();
        vertexSender.send(vertex);
    }

    // Run for some time to simulate the consensus algorithm
    std::cout << "Running the consensus algorithm for 10 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Retrieve and print output vertices
    std::cout << "Retrieving and printing output vertices..." << std::endl;
    while (auto vertex = vertexOutputReceiver.receive()) {
        std::cout << "Output Vertex: " << vertex->hash() << std::endl;
    }

    std::cout << "Main program finished." << std::endl;
    return 0;
}
