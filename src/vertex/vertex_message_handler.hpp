#ifndef VERTEX_MESSAGE_HANDLER_HPP
#define VERTEX_MESSAGE_HANDLER_HPP

#include <future>
#include <memory>

// #include "message_handler.hpp"
#include "receiver.hpp"
#include "vertex.hpp"

class VertexReceiverHandler : public MessageHandler {
public:
    explicit VertexReceiverHandler(
        std::shared_ptr<std::promise<Vertex>> vertex_to_consensus_promise)
        : vertex_to_consensus_promise_(vertex_to_consensus_promise) {}

    net::awaitable<void> dispatch(Writer& writer, const std::string& message);

private:
    std::shared_ptr<std::promise<Vertex>> vertex_to_consensus_promise_;
};

#endif  // VERTEX_MESSAGE_HANDLER_HPP
