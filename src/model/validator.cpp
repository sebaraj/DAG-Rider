#include "validator.hpp"
#include <blake3.h>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <json.hpp>

Validator::Validator(const std::string &keypair, uint16_t port,
                     uint16_t tx_port, uint16_t block_port) {
  ed25519_public_key public_key;
  ed25519_secret_key secret_key;
  create_keypair(keypair, public_key, secret_key);
  public_key_ = create_public_key_from(secret_key);

  address_ = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::address::from_string("0.0.0.0"), port);
  tx_address_ = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::address::from_string("0.0.0.0"), tx_port);
  block_address_ = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::address::from_string("0.0.0.0"), block_port);
}

void Validator::create_keypair(const std::string &kps,
                               ed25519_public_key &public_key,
                               ed25519_secret_key &secret_key) {
  std::vector<uint8_t> bytes =
      nlohmann::json::from_bson(kps).get<std::vector<uint8_t>>();
  std::copy(bytes.begin(), bytes.end(), secret_key);
  ed25519_publickey(secret_key, public_key);
}

NodePublicKey
Validator::create_public_key_from(const ed25519_secret_key &secret_key) {
  ed25519_public_key public_key;
  ed25519_publickey(secret_key, public_key);

  std::vector<uint8_t> encoded = nlohmann::json::to_cbor(public_key);
  NodePublicKey hash;
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);
  blake3_hasher_update(&hasher, encoded.data(), encoded.size());
  blake3_hasher_finalize(&hasher, hash.data(), hash.size());
  return hash;
}

void to_json(nlohmann::json &j, const Validator &validator) {
  j = nlohmann::json{
      {"address", validator.address_.address().to_string() + ":" +
                      std::to_string(validator.address_.port())},
      {"tx_address", validator.tx_address_.address().to_string() + ":" +
                         std::to_string(validator.tx_address_.port())},
      {"block_address", validator.block_address_.address().to_string() + ":" +
                            std::to_string(validator.block_address_.port())},
      {"public_key", validator.public_key_}};
}

void from_json(const nlohmann::json &j, Validator &validator) {
  std::string address_str = j.at("address").get<std::string>();
  std::string tx_address_str = j.at("tx_address").get<std::string>();
  std::string block_address_str = j.at("block_address").get<std::string>();
  validator.address_ = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::make_address(
          address_str.substr(0, address_str.find(":"))),
      std::stoi(address_str.substr(address_str.find(":") + 1)));
  validator.tx_address_ = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::make_address(
          tx_address_str.substr(0, tx_address_str.find(":"))),
      std::stoi(tx_address_str.substr(tx_address_str.find(":") + 1)));
  validator.block_address_ = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::make_address(
          block_address_str.substr(0, block_address_str.find(":"))),
      std::stoi(block_address_str.substr(block_address_str.find(":") + 1)));
  validator.public_key_ = j.at("public_key").get<NodePublicKey>();
}
