#include "state.hpp"

#include <unordered_map>

State::State(const std::vector<Vertex>& genesisVertices)
    : currentRound(1),
      deliveredVertices(),
      dag(genesisVertices, (2 * genesisVertices.size() / 3 + 1)) {
    for (const auto& vertex : genesisVertices) {
        deliveredVertices.insert(vertex.hash());
    }
}

void State::setVertexAsDelivered(const VertexHash& vertexHash) {
    deliveredVertices.insert(vertexHash);
}
