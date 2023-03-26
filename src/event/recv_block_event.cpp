#include "event/recv_block_event.h"

RecvBlockEvent::RecvBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Block> &block)
    : Event(timestamp), from(from), to(to), block(block) {}

void RecvBlockEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                             const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->insertBlock(block);
  uint64_t hash = block->hash();
  for (auto &[_, peer] : node->peerMap) {
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
