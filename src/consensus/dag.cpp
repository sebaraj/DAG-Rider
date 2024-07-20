#include "dag.hpp"

#include <stack>

Dag::Dag(const std::vector<Vertex>& root, uint32_t minQuorum) : minQuorum(minQuorum) {
    std::unordered_map<NodePublicKey, Vertex> genesis;
    for (const auto& v : root) {
        genesis[v.owner()] = v;
    }
    graph[1] = genesis;
}

void Dag::insertVertex(const Vertex& vertex) { graph[vertex.round()][vertex.owner()] = vertex; }

bool Dag::containsVertices(const std::map<VertexHash, Round>& vertices) const {
    for (const auto& [vertexHash, round] : vertices) {
        auto it = graph.find(round);
        if (it == graph.end()
            || std::none_of(it->second.begin(), it->second.end(), [vertexHash](const auto& pair) {
                   return pair.second.hash() == vertexHash;
               })) {
            return false;
        }
    }
    return true;
}

std::map<VertexHash, Round> Dag::getVertices(Round round) const {
    std::map<VertexHash, Round> result;
    auto it = graph.find(round);
    if (it != graph.end()) {
        for (const auto& [_, vertex] : it->second) {
            result[vertex.hash()] = vertex.round();
        }
    }
    return result;
}

bool Dag::isQuorumReachedForRound(Round round) const {
    auto it = graph.find(round);
    return it != graph.end() && it->second.size() >= minQuorum;
}

bool Dag::isLinkedWithOthersInRound(const Vertex& vertex, Round round) const {
    uint32_t weight = 0;
    auto it = graph.find(round);
    if (it != graph.end()) {
        for (const auto& [_, v] : it->second) {
            if (isStronglyLinked(v, vertex)) {
                ++weight;
            }
        }
    }
    return weight >= minQuorum;
}

bool Dag::isStronglyLinked(const Vertex& newest, const Vertex& oldest) const {
    return isLinkedInternal(newest, oldest, [](const Vertex& v) { return v.get_strong_parents(); });
}

bool Dag::isLinked(const Vertex& newest, const Vertex& oldest) const {
    return isLinkedInternal(newest, oldest, [](const Vertex& v) { return v.get_all_parents(); });
}

std::shared_ptr<Vertex> Dag::getVertex(VertexHash vertexHash, Round round) const {
    auto it = graph.find(round);
    if (it != graph.end()) {
        for (const auto& [_, vertex] : it->second) {
            if (vertex.hash() == vertexHash) {
                return std::make_shared<Vertex>(vertex);
            }
        }
    }
    return nullptr;
}

bool Dag::isLinkedInternal(
    const Vertex& newest, const Vertex& oldest,
    std::function<std::map<VertexHash, Round>(const Vertex&)> getParents) const {
    if (newest.round() > oldest.round()) {
        std::stack<Vertex> vertexStack;
        vertexStack.push(newest);

        while (!vertexStack.empty()) {
            Vertex vertex = vertexStack.top();
            vertexStack.pop();

            for (const auto& [parentHash, round] : vertex.get_all_parents()) {
                if (parentHash == oldest.hash()) {
                    return true;
                } else if (round > oldest.round()) {
                    auto parentVertex = getVertex(parentHash, round);
                    if (parentVertex) {
                        vertexStack.push(*parentVertex);
                    }
                }
            }
        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const Dag& dag) {
    std::unordered_map<VertexHash, int> vertexIds;
    for (const auto& [round, vertices] : dag.graph) {
        os << round << ": ";

        int c = 1;
        for (const auto& [_, vertex] : vertices) {
            vertexIds[vertex.hash()] = c;

            os << "(V" << c << ")";
            const auto& parents = vertex.get_all_parents();
            if (!parents.empty()) {
                os << "[";
                for (const auto& [hash, round] : parents) {
                    if (vertexIds.find(hash) != vertexIds.end()) {
                        os << round << "-" << vertexIds[hash] << " ";
                    }
                }
                os << "]";
            }
            if (c < vertices.size()) {
                os << " --- ";
            }

            ++c;
        }
        os << "\n";
    }
    return os;
}

const std::map<Round, std::unordered_map<NodePublicKey, Vertex>>& Dag::getGraph() const {
    return graph;
}
