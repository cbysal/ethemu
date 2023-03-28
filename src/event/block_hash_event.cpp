#include "event/block_hash_event.h"
#include "common/math.h"
#include "emu/config.h"

BlockHashEvent::BlockHashEvent(uint64_t timestamp, uint16_t from, uint16_t to, Hash blockHash)
    : Event(timestamp), from(from), to(to), blockHash(blockHash) {}

void BlockHashEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                             const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->fetchingHeaders[blockHash].push_back(from);
}

std::string BlockHashEvent::toString() const {
  return "BlockHashEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + hashHex(blockHash) + ")";
}
