#include "vertex_message_handler.hpp"

#include <boost/asio.hpp>
#include <iostream>

#include "vertex.hpp"

net::awaitable<void> VertexReceiverHandler::dispatch(Writer &writer,
                                                     const std::string &serialized) {
    std::cout << "Dispatching received vertex message" << std::endl;
    nlohmann::json j = nlohmann::json::parse(serialized);
    Vertex vertex = Vertex::from_json(j);
    vertex_to_consensus_promise_->set_value(vertex);

    co_return;
}
