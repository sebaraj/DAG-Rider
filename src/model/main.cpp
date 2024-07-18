#include "committee.hpp"
#include "vertex.hpp"
#include <iostream>
#include <json.hpp>

int main() {
  Committee committee;

  // Serialize committee to JSON
  nlohmann::json j = committee;
  std::cout << j.dump(4) << std::endl;

  // Deserialize JSON to committee
  Committee new_committee = j.get<Committee>();

  return 0;
}
