#include "validator.hpp"
#include <blake3.h>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <json.hpp>

Validator::Validator(const std::string &keypair, uint16_t port,
                     uint16_t tx_port, uint16_t block_port) {
  std::cout << "Creating Validator with keypair: " << keypair << std::endl;
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
  std::cout << "Creating keypair from hexadecimal string: " << kps << std::endl;
  try {
    if (kps.size() != 128) {
      throw std::invalid_argument("Invalid keypair length");
    }

    for (size_t i = 0; i < 32; ++i) {
      std::string byteString = kps.substr(2 * i, 2);
      secret_key[i] = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
    }
    std::cout << "Secret key created successfully" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error creating secret key: " << e.what() << std::endl;
    throw;
  }
  ed25519_publickey(secret_key, public_key);
  std::cout << "Public key created successfully" << std::endl;
}

NodePublicKey
Validator::create_public_key_from(const ed25519_secret_key &secret_key) {
  std::cout << "Creating public key from secret key" << std::endl;
  ed25519_public_key public_key;
  ed25519_publickey(secret_key, public_key);

  std::vector<uint8_t> encoded = nlohmann::json::to_cbor(public_key);
  NodePublicKey hash;
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);
  blake3_hasher_update(&hasher, encoded.data(), encoded.size());
  blake3_hasher_finalize(&hasher, hash.data(), hash.size());
  std::cout << "Public key hash created successfully" << std::endl;
  return hash;
}

void to_json(nlohmann::json &j, const Validator &validator) {
  std::cout << "Serializing Validator to JSON" << std::endl;
  j = nlohmann::json{
      {"address", validator.address_.address().to_string() + ":" +
                      std::to_string(validator.address_.port())},
      {"tx_address", validator.tx_address_.address().to_string() + ":" +
                         std::to_string(validator.tx_address_.port())},
      {"block_address", validator.block_address_.address().to_string() + ":" +
                            std::to_string(validator.block_address_.port())},
      {"public_key", validator.public_key_}};
  std::cout << "Validator serialized successfully" << std::endl;
}

void from_json(const nlohmann::json &j, Validator &validator) {
  std::cout << "Deserializing Validator from JSON" << std::endl;
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
  std::cout << "Validator deserialized successfully" << std::endl;
}
