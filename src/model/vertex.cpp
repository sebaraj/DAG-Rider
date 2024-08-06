#include "vertex.hpp"

#include <blake3.h>

#include <nlohmann/json.hpp>

Vertex::Vertex(NodePublicKey owner, Round round, const Block &block,
               const std::map<VertexHash, Round> &parents)
    : hash_(), owner_(owner), block_(block), parents_(parents), round_(round) {
    auto vertex_json = this->to_json();
    std::vector<uint8_t> encoded = nlohmann::json::to_cbor(vertex_json);
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, encoded.data(), encoded.size());
    blake3_hasher_finalize(&hasher, hash_.data(), hash_.size());
}

nlohmann::json Vertex::to_json() const {
    nlohmann::json j;
    j["hash"] = hash_;
    j["owner"] = owner_;
    j["block"] = block_;
    j["parents"] = parents_;
    j["round"] = round_;
    return j;
}

Vertex Vertex::from_json(const nlohmann::json &j) {
    Vertex vertex;

    std::string hash_str = j.at("hash").get<std::string>();
    if (hash_str.size() != vertex.hash_.size()) {
        throw std::invalid_argument("Invalid hash size");
    }
    std::copy(hash_str.begin(), hash_str.end(), vertex.hash_.begin());

    std::string owner_str = j.at("owner").get<std::string>();
    if (owner_str.size() != vertex.owner_.size()) {
        throw std::invalid_argument("Invalid owner size");
    }
    std::copy(owner_str.begin(), owner_str.end(), vertex.owner_.begin());

    vertex.block_ = j.at("block").get<Block>();

    for (auto &parent : j.at("parents").items()) {
        std::string parent_key_str = parent.key();
        VertexHash parent_key;
        if (parent_key_str.size() != parent_key.size()) {
            throw std::invalid_argument("Invalid parent key size");
        }
        std::copy(parent_key_str.begin(), parent_key_str.end(), parent_key.begin());
        vertex.parents_[parent_key] = parent.value().get<Round>();
    }

    vertex.round_ = j.at("round").get<Round>();

    return vertex;
}

std::vector<Vertex> Vertex::genesis(const std::vector<NodePublicKey> &nodes) {
    std::vector<Vertex> vertices;
    for (const auto &owner : nodes) {
        vertices.emplace_back(owner, 1, Block(), std::map<VertexHash, Round>());
    }
    return vertices;
}

void Vertex::add_parent(const VertexHash &parent_vertex_hash, Round round) {
    parents_[parent_vertex_hash] = round;
}

std::map<VertexHash, Round> Vertex::get_strong_parents() const {
    std::map<VertexHash, Round> strong_parents;
    for (const auto &parent : parents_) {
        if (is_previous_round(parent.second)) {
            strong_parents[parent.first] = parent.second;
        }
    }
    return strong_parents;
}

std::map<VertexHash, Round> Vertex::get_all_parents() const { return parents_; }

bool Vertex::is_weak_parent(const VertexHash &vertex_hash) const {
    auto it = parents_.find(vertex_hash);
    return it != parents_.end() && !is_previous_round(it->second);
}

Round Vertex::round() const { return round_; }

const std::map<VertexHash, Round> &Vertex::parents() const { return parents_; }

NodePublicKey Vertex::owner() const { return owner_; }

VertexHash Vertex::hash() const { return hash_; }

bool Vertex::is_previous_round(Round previous_round) const { return round_ - previous_round == 1; }

std::ostream &operator<<(std::ostream &os, const VertexHash &hash) {
    os << std::hex;
    for (const auto &byte : hash) {
        os << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    os << std::dec;
    return os;
}
