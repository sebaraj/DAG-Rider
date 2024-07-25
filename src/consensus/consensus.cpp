#include "consensus.hpp"

#include <iostream>
#include <thread>
/*
namespace std {
template <>
struct hash<VertexHash> {
    std::size_t operator()(const VertexHash& key) const noexcept {
        std::size_t hash = 0;
        for (const auto& byte : key) {
            hash ^= std::hash<uint8_t>{}(byte) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};
}*/  // namespace std

void Consensus::spawn(NodePublicKey nodeId, Committee committee, Receiver<Vertex>& vertexReceiver,
                      Sender<Vertex>& vertexToBroadcastSender, Sender<Vertex>& vertexOutputSender,
                      Receiver<Block>& blocksReceiver) {
    std::vector<Vertex> genesisVertices = Vertex::genesis(committee.get_nodes_keys());
    Consensus consensus(nodeId, committee, genesisVertices, vertexReceiver, vertexToBroadcastSender,
                        vertexOutputSender, blocksReceiver);
    std::thread([&consensus] { consensus.run(); }).detach();
}

Consensus::Consensus(NodePublicKey nodeId, Committee committee,
                     const std::vector<Vertex>& genesisVertices, Receiver<Vertex>& vertexReceiver,
                     Sender<Vertex>& vertexToBroadcastSender, Sender<Vertex>& vertexOutputSender,
                     Receiver<Block>& blocksReceiver)
    : nodeId(std::move(nodeId)),
      committee(std::move(committee)),
      state(genesisVertices),
      decidedWave(0),
      blocksToPropose(std::vector<Block>()),
      vertexReceiver(vertexReceiver),
      vertexOutputSender(vertexOutputSender),
      vertexToBroadcastSender(vertexToBroadcastSender),
      blocksReceiver(blocksReceiver) {}

void Consensus::run() {
    std::cout << "Running consensus algorithm..." << std::endl;
    while (true) {
        std::vector<Vertex> buffer;

        while (auto vertex = vertexReceiver.receive()) {
            buffer.push_back(std::move(*vertex));
        }

        while (auto block = blocksReceiver.receive()) {
            blocksToPropose.push_back(std::move(*block));
        }

        for (const auto& vertex : buffer) {
            if (vertex.round() <= state.currentRound
                && state.dag.containsVertices(vertex.parents())) {
                state.dag.insertVertex(vertex);
            }
        }

        if (!blocksToPropose.empty() && state.dag.isQuorumReachedForRound(state.currentRound)) {
            if (isLastRoundInWave(state.currentRound)) {
                auto orderedVertices = getOrderedVertices(state.currentRound / MAX_WAVE);
                for (const auto& vertex : orderedVertices) {
                    vertexOutputSender.send(vertex);
                }
            }
            state.currentRound++;
            createNewVertex(state.currentRound);
        }
    }
}

void Consensus::createNewVertex(Round round) {
    if (blocksToPropose.empty()) {
        return;
    }

    auto block = blocksToPropose.back();
    blocksToPropose.pop_back();

    auto parents = state.dag.getVertices(round - 1);
    Vertex vertex(nodeId, round, block, parents);

    if (round > 2) {
        setWeakEdges(vertex, round);
    }

    vertexToBroadcastSender.send(vertex);
}

void Consensus::setWeakEdges(Vertex& vertex, Round round) {
    for (Round r = round - 3; r > 0; --r) {
        if (auto it = state.dag.getGraph().find(r); it != state.dag.getGraph().end()) {
            for (const auto& [_, v] : it->second) {
                if (!state.dag.isLinked(vertex, v)) {
                    vertex.add_parent(v.hash(), r);
                }
            }
        }
    }
}

std::vector<Vertex> Consensus::getOrderedVertices(Wave wave) {
    auto leader = getWaveVertexLeader(wave);
    if (!leader) {
        return {};
    }

    Round round = getRoundForWave(wave, MAX_WAVE);
    if (!state.dag.isLinkedWithOthersInRound(*leader, round)) {
        return {};
    }

    auto leadersToCommit = getLeadersToCommit(wave - 1, *leader);
    decidedWave = wave;

    return orderVertices(leadersToCommit);
}

std::vector<Vertex> Consensus::getLeadersToCommit(Wave fromWave, const Vertex& currentLeader) {
    std::vector<Vertex> toCommit{currentLeader};
    const Vertex* leader = &currentLeader;

    for (Wave wave = fromWave; wave > 0; --wave) {
        auto prevLeader = getWaveVertexLeader(wave);
        if (prevLeader && state.dag.isStronglyLinked(*leader, *prevLeader)) {
            toCommit.push_back(*prevLeader);
            leader = prevLeader.get();
        }
    }
    return toCommit;
}

std::vector<Vertex> Consensus::orderVertices(std::vector<Vertex>& leaders) {
    std::vector<Vertex> orderedVertices;
    std::unordered_set<VertexHash> deliveredVertices;

    while (!leaders.empty()) {
        auto leader = std::move(leaders.back());
        leaders.pop_back();

        for (const auto& [round, vertices] : state.dag.getGraph()) {
            if (round > 0) {
                for (const auto& [_, vertex] : vertices) {
                    if (!deliveredVertices.contains(vertex.hash())
                        && state.dag.isLinked(vertex, leader)) {
                        orderedVertices.push_back(vertex);
                        deliveredVertices.insert(vertex.hash());
                    }
                }
            }
        }
    }
    return orderedVertices;
}

std::shared_ptr<Vertex> Consensus::getWaveVertexLeader(Wave wave) const {
    Round firstRoundOfWave = getRoundForWave(wave, 1);
    auto coin = firstRoundOfWave;

    auto keys = committee.get_nodes_keys();
    std::sort(keys.begin(), keys.end());

    auto leader = keys[coin % committee.size()];

    auto it = state.dag.getGraph().find(firstRoundOfWave);
    if (it != state.dag.getGraph().end()) {
        if (auto vertexIt = it->second.find(leader); vertexIt != it->second.end()) {
            return std::make_shared<Vertex>(vertexIt->second);
        }
    }
    return nullptr;
}

Round Consensus::getRoundForWave(Wave wave, Round round) const {
    return (MAX_WAVE * (wave - 1) + round);
}

bool Consensus::isLastRoundInWave(Round round) { return round % MAX_WAVE == 0; }
