#include "block.hpp"

#include <blake3.h>

#include <json.hpp>

Block::Block(const std::vector<Transaction> &transactions) : transactions_(transactions) {
    auto encoded = nlohmann::json::to_cbor(transactions_);
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, encoded.data(), encoded.size());
    blake3_hasher_finalize(&hasher, hash_.data(), hash_.size());
}

BlockHash Block::hash() const { return hash_; }

void to_json(nlohmann::json &j, const Block &block) {
    j = nlohmann::json{{"hash", block.hash_}, {"transactions", block.transactions_}};
}

void from_json(const nlohmann::json &j, Block &block) {
    j.at("hash").get_to(block.hash_);
    j.at("transactions").get_to(block.transactions_);
}
