#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <array>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <ed25519-donna/ed25519.h>
#include <json.hpp>
#include <string>

using Id = uint32_t;
using NodePublicKey = std::array<uint8_t, 32>;

class Validator {
public:
  Validator() = default;
  Validator(const std::string &keypair, uint16_t port, uint16_t tx_port,
            uint16_t block_port);
  void set(const std::string &keypair, uint16_t port, uint16_t tx_port,
           uint16_t block_port);
  static NodePublicKey
  create_public_key_from(const ed25519_secret_key &secret_key);

  boost::asio::ip::tcp::endpoint address_;
  boost::asio::ip::tcp::endpoint tx_address_;
  boost::asio::ip::tcp::endpoint block_address_;
  NodePublicKey public_key_;

  // JSON serialization and deserialization
  friend void to_json(nlohmann::json &j, const Validator &validator);
  friend void from_json(const nlohmann::json &j, Validator &validator);

private:
  static void create_keypair(const std::string &kps,
                             ed25519_public_key &public_key,
                             ed25519_secret_key &secret_key);
};

void to_json(nlohmann::json &j, const Validator &validator);
void from_json(const nlohmann::json &j, Validator &validator);

#endif // VALIDATOR_HPP
