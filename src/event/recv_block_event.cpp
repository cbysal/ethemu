#include "event/recv_block_event.h"

RecvBlockEvent::RecvBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, Block *block) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->block = block;
}

void RecvBlockEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db,
                             const std::vector<uint64_t> &nodeList,
                             const std::unordered_map<uint64_t, Node *> &nodeMap) {
  Node *node = nodeMap.at(to);
  node->insertBlock(block);
  for (auto peer : node->peerList) {
    if (!peer->knownBlock(block->hash())) {
      queue.push(new SendBlockEvent(timestamp, to, peer->addr, block));
      peer->markBlock(block->hash());
    }
  }
}

std::string RecvBlockEvent::toString() const {
  return "RecvBlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", block: " + block->hash().toString() + ")";
}
