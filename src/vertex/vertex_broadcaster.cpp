#include "vertex_broadcaster.hpp"

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <thread>

VertexBroadcaster::VertexBroadcaster(
    std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future,
    std::shared_ptr<Broadcaster> network, Committee committee)
    : vertex_to_broadcast_future_(vertex_to_broadcast_future),
      network_(network),
      committee_(committee) {}

void VertexBroadcaster::spawn(std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future,
                              std::shared_ptr<Broadcaster> network, Committee committee) {
    auto broadcaster
        = std::make_shared<VertexBroadcaster>(vertex_to_broadcast_future, network, committee);
    std::thread(&VertexBroadcaster::run, broadcaster).detach();
}

void VertexBroadcaster::run() {
    while (true) {
        Vertex vertex = vertex_to_broadcast_future_->get();
        // std::cout << "Vertex received for broadcast " << vertex << std::endl;
        auto addresses = committee_.get_node_addresses();
        auto bytes = vertex.to_json();

        for (const auto& endpoint : addresses) {
            auto address = endpoint.address().to_string();
            auto port = endpoint.port();
            net::co_spawn(network_->get_io_context(), network_->send(address, port, bytes),
                          net::detached);
        }

        /*auto handlers = network_->send(addresses, bytes);
        for (auto& h : handlers) {
            if (h.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
                std::cerr << "Broadcast of vertices was not successful" << std::endl;
            }
        }*/
    }
}
