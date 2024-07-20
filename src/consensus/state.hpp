#ifndef STATE_HPP
#define STATE_HPP

#include <unordered_set>
#include <vector>

#include "dag.hpp"
#include "hash.hpp"
#include "vertex.hpp"

class State {
public:
    State(const std::vector<Vertex>& genesisVertices);

    void setVertexAsDelivered(const VertexHash& vertexHash);

    uint32_t currentRound;
    std::unordered_set<VertexHash> deliveredVertices;
    Dag dag;
};

#endif  // STATE_HPP
