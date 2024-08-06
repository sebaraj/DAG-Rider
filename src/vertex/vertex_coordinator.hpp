#ifndef VERTEX_COORDINATOR_HPP
#define VERTEX_COORDINATOR_HPP

#include <future>
#include <memory>

#include "committee.hpp"
#include "receiver.hpp"
#include "vertex.hpp"
#include "vertex_broadcaster.hpp"
#include "vertex_message_handler.hpp"

class VertexCoordinator {
public:
    static void spawn(Id node_id, Committee committee,
                      std::shared_ptr<std::promise<Vertex>> vertex_to_consensus_promise,
                      std::shared_ptr<std::future<Vertex>> vertex_to_broadcast_future);
};

#endif  // VERTEX_COORDINATOR_HPP
