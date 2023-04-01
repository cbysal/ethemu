#include "event/header_event.h"
#include "common/math.h"
#include "emu/config.h"
#include "event/body_fetch_timer_event.h"

HeaderEvent::HeaderEvent(uint64_t timestamp, Id from, Id to, const std::shared_ptr<Header> &header)
    : Event(timestamp), from(from), to(to), header(header) {}

void HeaderEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                          const std::vector<Node *> &nodes) const {
  Node *node = nodes[to];
  uint64_t blockHash = header->hash();
  node->fetchedHeaders[blockHash] = header;
  node->fetchingBodies[blockHash].push_back(from);
  if (node->fetchedHeaders.size() == 1)
    node->nextFetchBodyTime = timestamp + bodyFetchInterval;
}

std::string HeaderEvent::toString() const {
  return "HeaderEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", header: " + hashHex(header->hash()) + ")";
}
