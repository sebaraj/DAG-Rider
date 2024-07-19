#include <iostream>
#include <json.hpp>

#include "committee.hpp"
#include "vertex.hpp"

int main() {
    try {
        Committee *committee = new Committee();
        // Committee committee;
        std::cout << "Committee created successfully" << std::endl;

        // Serialize committee to JSON
        nlohmann::json j = *committee;
        std::cout << "Committee serialized to JSON" << std::endl;
        std::cout << j.dump(4) << std::endl;

        // Deserialize JSON to committee
        Committee *new_committee = new Committee(j.get<Committee>());
        // Committee new_committee = j.get<Committee>();
        std::cout << "Committee deserialized from JSON" << std::endl;
        delete committee;
        delete new_committee;
    } catch (const std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown error occurred" << std::endl;
    }

    return 0;
}
