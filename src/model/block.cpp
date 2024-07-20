#include "block.hpp"

#include <blake3.h>

#include <nlohmann/json.hpp>

Block::Block(const std::vector<Transaction> &transactions) : transactions_(transactions) {
    auto encoded = nlohmann::json::to_cbor(transactions_);
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, encoded.data(), encoded.size());
    blake3_hasher_finalize(&hasher, hash_.data(), hash_.size());
}

void Block::addTransaction(const std::string &transaction) {
    transactions_.emplace_back(transaction.begin(), transaction.end());
    // Recompute hash after adding transaction
    nlohmann::json j = *this;
    std::vector<uint8_t> encoded = nlohmann::json::to_cbor(j);
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
