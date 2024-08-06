#include "committee.hpp"
#include "vertex.hpp"
#include "vertex_coordinator.hpp"

int main() {
    Id node_id = 1;
    Committee committee;
    auto vertex_to_consensus_promise = std::make_shared<std::promise<Vertex>>();
    auto vertex_to_broadcast_future
        = std::make_shared<std::future<Vertex>>(vertex_to_consensus_promise->get_future());
    std::cout << "reached" << std::endl;

    VertexCoordinator::spawn(node_id, committee, vertex_to_consensus_promise,
                             vertex_to_broadcast_future);
    return 0;
}
