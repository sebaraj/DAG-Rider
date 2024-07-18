#include "committee.hpp"

Committee::Committee() {
  validators_.emplace(
      1, Validator(
             "ad7f2ee3958a7f3fa2c84931770f5773ef7694fdd0bb217d90f29a94199c9d730"
             "7ca3851515c89344639fe6a4077923068d1d7fc6106701213c61d34ef8e9416",
             1234, 1244, 1254));
  validators_.emplace(
      2, Validator(
             "5a353c630d3faf8e2d333a0983c1c71d5e9b6aed8f4959578fbeb3d3f31728863"
             "93b576de0ac1fe86a4dd416cf032543ac1bd066eb82585f779f6ce21237c0cd",
             1235, 1245, 1255));
  validators_.emplace(
      3, Validator(
             "6f4b736b9a6894858a81696d9c96cbdacf3d49099d212213f5abce33da18716f0"
             "67f8a2b9aeb602cd4163291ebbf39e0e024634f3be19bde4c490465d9095a6b",
             1236, 1246, 1256));
  validators_.emplace(
      4, Validator(
             "3ae38eec96146c241f6cadf01995af14f027b23b8fecbc77dbc2e3ed5fec6fc3f"
             "b4fe5534f7affc9a8f1d99e290fdb91cc26777edd6fae480cad9f735d1b3680",
             1237, 1247, 1257));
}

size_t Committee::size() const { return validators_.size(); }

size_t Committee::quorum_threshold() const { return size() - 1; }

boost::asio::ip::tcp::endpoint Committee::get_node_address(Id id) const {
  auto it = validators_.find(id);
  if (it != validators_.end()) {
    return it->second.address_;
  }
  return boost::asio::ip::tcp::endpoint();
}

std::vector<boost::asio::ip::tcp::endpoint>
Committee::get_node_addresses() const {
  std::vector<boost::asio::ip::tcp::endpoint> addresses;
  for (const auto &validator : validators_) {
    addresses.push_back(validator.second.address_);
  }
  return addresses;
}

boost::asio::ip::tcp::endpoint Committee::get_tx_receiver_address(Id id) const {
  auto it = validators_.find(id);
  if (it != validators_.end()) {
    return it->second.tx_address_;
  }
  return boost::asio::ip::tcp::endpoint();
}

std::vector<boost::asio::ip::tcp::endpoint>
Committee::get_tx_receiver_addresses() const {
  std::vector<boost::asio::ip::tcp::endpoint> addresses;
  for (const auto &validator : validators_) {
    addresses.push_back(validator.second.tx_address_);
  }
  return addresses;
}

boost::asio::ip::tcp::endpoint
Committee::get_block_receiver_address(Id id) const {
  auto it = validators_.find(id);
  if (it != validators_.end()) {
    return it->second.block_address_;
  }
  return boost::asio::ip::tcp::endpoint();
}

std::vector<boost::asio::ip::tcp::endpoint>
Committee::get_block_receiver_addresses() const {
  std::vector<boost::asio::ip::tcp::endpoint> addresses;
  for (const auto &validator : validators_) {
    addresses.push_back(validator.second.block_address_);
  }
  return addresses;
}

std::vector<boost::asio::ip::tcp::endpoint>
Committee::get_node_addresses_but_me(Id id) const {
  std::vector<boost::asio::ip::tcp::endpoint> addresses;
  for (const auto &validator : validators_) {
    if (validator.first != id) {
      addresses.push_back(validator.second.address_);
    }
  }
  return addresses;
}

std::vector<NodePublicKey> Committee::get_nodes_keys() const {
  std::vector<NodePublicKey> keys;
  for (const auto &validator : validators_) {
    keys.push_back(validator.second.public_key_);
  }
  return keys;
}

NodePublicKey Committee::get_node_key(Id id) const {
  auto it = validators_.find(id);
  if (it != validators_.end()) {
    return it->second.public_key_;
  }
  return NodePublicKey();
}

void to_json(nlohmann::json &j, const Committee &committee) {
  j = nlohmann::json{{"validators", committee.validators_}};
}

void from_json(const nlohmann::json &j, Committee &committee) {
  j.at("validators").get_to(committee.validators_);
}
