#ifndef HASH_HPP
#define HASH_HPP

#include <array>
#include <cstddef>
#include <functional>

// Define the common hash function for std::array<uint8_t, 32>
template <std::size_t N>
struct ArrayHash {
    std::size_t operator()(const std::array<uint8_t, N>& key) const noexcept {
        std::size_t hash = 0;
        for (const auto& byte : key) {
            hash ^= std::hash<uint8_t>{}(byte) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

namespace std {
template <>
struct hash<std::array<uint8_t, 32>> : ArrayHash<32> {};
}  // namespace std

#endif  // HASH_HPP
