#include "vertex_coordinator.hpp"

#include <iostream>

#include "receiver.hpp"
#include "vertex_message_handler.hpp"

void VertexCoordinator::spawn(Id node_id, Committee committee,
                              std::shared_ptr<std::promise<Vertex>> vertex_to_consensus_promise,
                              std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future) {
    // Spawn the network receiver listening to vertices broadcasted from the other nodes.
    std::cout << "Start listening for vertices from other nodes" << std::endl;
    auto endpoint = committee.get_node_address(node_id);
    std::string address = endpoint.address().to_string();
    unsigned short port = endpoint.port();

    auto handler = std::make_shared<VertexReceiverHandler>(vertex_to_consensus_promise);
    auto receiver = std::make_shared<Receiver<VertexReceiverHandler>>(address, port, handler);
    receiver->start();

    std::cout << "Vertex Coordinator listening to the messages on " << address << ":" << port
              << std::endl;

    VertexBroadcaster::spawn(vertex_to_broadcast_future, std::make_shared<Broadcaster>(),
                             committee);
}
