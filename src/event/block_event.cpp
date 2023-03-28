#include "event/block_event.h"
#include "common/math.h"
#include "emu/config.h"
#include "event/block_hash_event.h"

BlockEvent::BlockEvent(uint64_t timestamp, uint16_t from, uint16_t to, const std::shared_ptr<Block> &block)
    : Event(timestamp), from(from), to(to), block(block) {}

void BlockEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                         const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->insertBlock(block);
  uint64_t hash = block->hash();
  std::vector<Peer *> peersWithoutBlock;
  for (auto &[_, peer] : node->peerMap)
    if (!peer->knownBlock(hash))
      peersWithoutBlock.push_back(peer);
  for (int i = 0; i < std::sqrt(peersWithoutBlock.size()); i++) {
    Peer *peer = peersWithoutBlock[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new BlockEvent(timestamp + interval, to, peer->id, block));
    peer->markBlock(hash);
  }
  for (int i = std::sqrt(peersWithoutBlock.size()); i < peersWithoutBlock.size(); i++) {
    Peer *peer = peersWithoutBlock[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new BlockHashEvent(timestamp + interval, to, peer->id, hash));
    peer->markBlock(hash);
  }
}

std::string BlockEvent::toString() const {
  return "BlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", block: " + hashHex(block->hash()) + ")";
}
