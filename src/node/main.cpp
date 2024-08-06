#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <thread>

#include "client.hpp"
#include "committee.hpp"
#include "receiver.hpp"
#include "vertex.hpp"
#include "vertex_coordinator.hpp"

using namespace boost::program_options;

/*void wait_and_print_vertices(std::shared_ptr<Receiver<Vertex>> vertex_output_receiver) {
    auto& io_context = vertex_output_receiver->get_executor();

    net::co_spawn(
        io_context,
        [vertex_output_receiver]() -> net::awaitable<void> {
            for (;;) {
                auto vertex_opt = co_await vertex_output_receiver->??();
                if (vertex_opt) {
                    std::cout << "Vertex committed: " << vertex_opt->to_json() << std::endl;
                }
            }
        },
        net::detached);

    io_context.run();
}*/

int main(int argc, char* argv[]) {
    try {
        options_description desc{"Options"};
        desc.add_options()("help,h", "Help screen")("id", value<int>()->required(), "Node ID")(
            "address", value<std::string>()->default_value("127.0.0.1"), "Node address")(
            "port", value<int>()->default_value(12345), "Node port");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }
        int node_id = vm["id"].as<int>();
        std::string address = vm["address"].as<std::string>();
        int port = vm["port"].as<int>();

        std::cout << "Node ID: " << node_id << std::endl;
        std::cout << "Address: " << address << std::endl;
        std::cout << "Port: " << port << std::endl;

        boost::asio::io_context io_context;

        Client client(io_context, address, port);
        client.send();

        auto vertex_to_consensus_promise = std::make_shared<std::promise<Vertex>>();
        auto vertex_to_consensus_future = vertex_to_consensus_promise->get_future();

        auto vertex_receiver_handler
            = std::make_shared<VertexReceiverHandler>(vertex_to_consensus_promise);
        auto vertex_to_broadcast_receiver = std::make_shared<Receiver<VertexReceiverHandler>>(
            address, port, vertex_receiver_handler);

        VertexCoordinator::spawn(
            node_id, Committee(), vertex_to_consensus_promise,
            std::make_shared<std::future<Vertex>>(std::move(vertex_to_consensus_future)));

        // std::thread vertex_thread(wait_and_print_vertices, vertex_to_broadcast_receiver);

        io_context.run();
        // vertex_thread.join();
    } catch (const boost::program_options::error& ex) {
        std::cerr << "Program options error: " << ex.what() << std::endl;
    } catch (const error& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}
