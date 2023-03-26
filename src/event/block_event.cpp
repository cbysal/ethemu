#include "emu/config.h"
#include "event/block_event.h"

BlockEvent::BlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Block> &block)
    : Event(timestamp), from(from), to(to), block(block) {}

void BlockEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                         const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->insertBlock(block);
  uint64_t hash = block->hash();
  for (auto &[_, peer] : node->peerMap) {
    if (!peer->knownBlock(hash)) {
      uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
      queue.push(new BlockEvent(timestamp + interval, to, peer->addr, block));
      peer->markBlock(hash);
    }
  }
}

std::string BlockEvent::toString() const {
  return "BlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", block: " + u64ToHex(block->hash()) + ")";
}
