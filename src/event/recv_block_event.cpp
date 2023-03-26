#include "event/recv_block_event.h"

RecvBlockEvent::RecvBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, Block *block) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->block = block;
}

void RecvBlockEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                             const std::vector<Node *> &nodes) {
  Node *node = nodes[to];
  node->insertBlock(block);
  uint64_t hash = block->hash();
  for (auto peer : node->peerList) {
    if (!peer->knownBlock(hash)) {
      queue.push(new SendBlockEvent(timestamp, to, peer->addr, block));
      peer->markBlock(hash);
    }
  }
}

std::string RecvBlockEvent::toString() const {
  return "RecvBlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", block: " + u64ToHex(block->hash()) + ")";
}
