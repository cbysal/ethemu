#include "event/block_hash_event.h"
#include "emu/config.h"

BlockHashEvent::BlockHashEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t blockHash)
    : Event(timestamp), from(from), to(to), blockHash(blockHash) {}

void BlockHashEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                             const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->fetchingHeaders[blockHash].push_back(from);
}

std::string BlockHashEvent::toString() const {
  return "BlockHashEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + u64ToHex(blockHash) + ")";
}
