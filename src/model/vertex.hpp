#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <cstdint>
#include <map>
#include <nlohmann/json.hpp>

#include "block.hpp"
#include "validator.hpp"

using VertexHash = std::array<uint8_t, 32>;
using Round = uint64_t;

class Vertex {
public:
    Vertex() = default;
    Vertex(NodePublicKey owner, Round round, const Block &block,
           const std::map<VertexHash, Round> &parents);
    static std::vector<Vertex> genesis(const std::vector<NodePublicKey> &nodes);
    void add_parent(const VertexHash &parent_vertex_hash, Round round);
    std::map<VertexHash, Round> get_strong_parents() const;
    std::map<VertexHash, Round> get_all_parents() const;
    bool is_weak_parent(const VertexHash &vertex_hash) const;
    Round round() const;
    const std::map<VertexHash, Round> &parents() const;
    NodePublicKey owner() const;
    VertexHash hash() const;

    // Convert Vertex to JSON
    nlohmann::json to_json() const;

private:
    VertexHash hash_;
    NodePublicKey owner_;
    Block block_;
    std::map<VertexHash, Round> parents_;
    Round round_;

    bool is_previous_round(Round previous_round) const;
};

std::ostream &operator<<(std::ostream &os, const VertexHash &hash);

#endif  // VERTEX_HPP
