#ifndef COMMITTEE_HPP
#define COMMITTEE_HPP

#include "validator.hpp"
#include <json.hpp>
#include <unordered_map>

class Committee {
public:
  Committee();
  size_t size() const;
  size_t quorum_threshold() const;
  boost::asio::ip::tcp::endpoint get_node_address(Id id) const;
  std::vector<boost::asio::ip::tcp::endpoint> get_node_addresses() const;
  boost::asio::ip::tcp::endpoint get_tx_receiver_address(Id id) const;
  std::vector<boost::asio::ip::tcp::endpoint> get_tx_receiver_addresses() const;
  boost::asio::ip::tcp::endpoint get_block_receiver_address(Id id) const;
  std::vector<boost::asio::ip::tcp::endpoint>
  get_block_receiver_addresses() const;
  std::vector<boost::asio::ip::tcp::endpoint>
  get_node_addresses_but_me(Id id) const;
  std::vector<NodePublicKey> get_nodes_keys() const;
  NodePublicKey get_node_key(Id id) const;

  // JSON serialization and deserialization
  friend void to_json(nlohmann::json &j, const Committee &committee);
  friend void from_json(const nlohmann::json &j, Committee &committee);

private:
  std::unordered_map<Id, Validator> validators_;
};

void to_json(nlohmann::json &j, const Committee &committee);
void from_json(const nlohmann::json &j, Committee &committee);

#endif // COMMITTEE_HPP
