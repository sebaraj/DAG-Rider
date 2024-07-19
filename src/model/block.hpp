#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <nlohmann/json.hpp>
#include <vector>

using Transaction = std::vector<uint8_t>;
using BlockHash = std::array<uint8_t, 32>;

class Block {
public:
    Block() = default;
    Block(const std::vector<Transaction> &transactions);

    BlockHash hash() const;

    std::vector<Transaction> transactions_;

    // JSON serialization and deserialization
    friend void to_json(nlohmann::json &j, const Block &block);
    friend void from_json(const nlohmann::json &j, Block &block);

private:
    BlockHash hash_;
};

void to_json(nlohmann::json &j, const Block &block);
void from_json(const nlohmann::json &j, Block &block);

#endif  // BLOCK_HPP
