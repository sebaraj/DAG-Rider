#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "committee.hpp"
#include "hash.hpp"
#include "vertex.hpp"

class Dag {
public:
    Dag(const std::vector<Vertex>& root, uint32_t minQuorum);
    void insertVertex(const Vertex& vertex);
    bool containsVertices(const std::map<VertexHash, Round>& vertices) const;
    std::map<VertexHash, Round> getVertices(Round round) const;
    bool isQuorumReachedForRound(Round round) const;
    bool isLinkedWithOthersInRound(const Vertex& vertex, Round round) const;
    bool isStronglyLinked(const Vertex& newest, const Vertex& oldest) const;
    bool isLinked(const Vertex& newest, const Vertex& oldest) const;
    std::shared_ptr<Vertex> getVertex(VertexHash vertexHash, Round round) const;
    friend std::ostream& operator<<(std::ostream& os, const Dag& dag);
    const std::map<Round, std::unordered_map<NodePublicKey, Vertex>>& getGraph() const;

private:
    std::map<Round, std::unordered_map<NodePublicKey, Vertex>> graph;
    uint32_t minQuorum;

    bool isLinkedInternal(
        const Vertex& newest, const Vertex& oldest,
        std::function<std::map<VertexHash, Round>(const Vertex&)> getParents) const;
};
