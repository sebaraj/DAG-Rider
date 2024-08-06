#ifndef VERTEX_BROADCASTER_HPP
#define VERTEX_BROADCASTER_HPP

#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "broadcaster.hpp"
#include "committee.hpp"
#include "vertex.hpp"

class VertexBroadcaster {
public:
    VertexBroadcaster(std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future,
                      std::shared_ptr<Broadcaster> network, Committee committee);
    static void spawn(std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future,
                      std::shared_ptr<Broadcaster> network, Committee committee);

    void run();

private:
    std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future_;
    std::shared_ptr<Broadcaster> network_;
    Committee committee_;
};

#endif  // VERTEX_BROADCASTER_HPP
