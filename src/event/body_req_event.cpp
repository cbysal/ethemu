#include "event/body_req_event.h"
#include "emu/config.h"
#include "event/body_event.h"

BodyReqEvent::BodyReqEvent(uint64_t timestamp, uint16_t from, uint16_t to, uint64_t blockHash)
    : Event(timestamp), from(from), to(to), blockHash(blockHash) {}

void BodyReqEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                           const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  std::shared_ptr<Block> block = node->blocksByHash[blockHash];
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new BodyEvent(timestamp + interval, to, from, blockHash, block->txs));
}

std::string BodyReqEvent::toString() const {
  return "BodyReqEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + u64ToHex(blockHash) + ")";
}
