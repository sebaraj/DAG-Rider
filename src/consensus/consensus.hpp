#pragma once

#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "block.hpp"
#include "committee.hpp"
#include "communication.hpp"
#include "hash.hpp"
#include "state.hpp"
#include "vertex.hpp"

using Wave = uint32_t;
const Wave MAX_WAVE = 4;

class Consensus {
public:
    static void spawn(NodePublicKey nodeId, Committee committee, Receiver<Vertex>& vertexReceiver,
                      Sender<Vertex>& vertexToBroadcastSender, Sender<Vertex>& vertexOutputSender,
                      Receiver<Block>& blocksReceiver);

    Consensus(NodePublicKey nodeId, Committee committee, const std::vector<Vertex>& genesisVertices,
              Receiver<Vertex>& vertexReceiver, Sender<Vertex>& vertexToBroadcastSender,
              Sender<Vertex>& vertexOutputSender, Receiver<Block>& blocksReceiver);

    void run();

    void createNewVertex(Round round);
    void setWeakEdges(Vertex& vertex, Round round);
    std::vector<Vertex> getOrderedVertices(Wave wave);
    std::vector<Vertex> getLeadersToCommit(Wave fromWave, const Vertex& currentLeader);
    std::vector<Vertex> orderVertices(std::vector<Vertex>& leaders);
    std::shared_ptr<Vertex> getWaveVertexLeader(Wave wave) const;
    Round getRoundForWave(Wave wave, Round round) const;
    static bool isLastRoundInWave(Round round);

private:
    NodePublicKey nodeId;
    Committee committee;
    State state;
    Wave decidedWave;

    std::vector<Block> blocksToPropose;
    Receiver<Vertex>& vertexReceiver;
    Sender<Vertex>& vertexOutputSender;
    Sender<Vertex>& vertexToBroadcastSender;
    Receiver<Block>& blocksReceiver;
};
