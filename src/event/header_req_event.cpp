#include "event/header_req_event.h"
#include "common/math.h"
#include "emu/config.h"
#include "event/header_event.h"

HeaderReqEvent::HeaderReqEvent(uint64_t timestamp, uint16_t from, uint16_t to, Hash blockHash)
    : Event(timestamp), from(from), to(to), blockHash(blockHash) {}

void HeaderReqEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                             const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  std::shared_ptr<Block> block = node->blocksByHash[blockHash];
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new HeaderEvent(timestamp + interval, to, from, block->header));
}

std::string HeaderReqEvent::toString() const {
  return "HeaderReqEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + hashHex(blockHash) + ")";
}
