#include <iostream>

#include "Coordinator.hpp"
#include "committee.hpp"

int main() {
    std::cout << "Starting Coordinator..." << std::endl;

    Id node_id = 0;       // Example node ID, replace with actual logic
    Committee committee;  // Initialize committee with necessary data

    Coordinator::spawn(node_id, committee);

    std::cout << "Coordinator started successfully!" << std::endl;

    return 0;
}
