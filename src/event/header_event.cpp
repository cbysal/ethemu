#include "event/header_event.h"
#include "emu/config.h"
#include "event/block_hash_event.h"

HeaderEvent::HeaderEvent(uint64_t timestamp, uint16_t from, uint16_t to, const std::shared_ptr<Header> &header)
    : Event(timestamp), from(from), to(to), header(header) {}

void HeaderEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                          const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  uint64_t blockHash = header->hash();
  node->fetchedHeaders[blockHash] = header;
  node->fetchingBodies[blockHash].push_back(from);
}

std::string HeaderEvent::toString() const {
  return "HeaderEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", header: " + u64ToHex(header->hash()) + ")";
}
