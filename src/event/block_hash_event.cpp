#include "event/block_hash_event.h"
#include "emu/config.h"
#include "event/block_req_event.h"

BlockHashEvent::BlockHashEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t blockHash)
    : Event(timestamp), from(from), to(to), blockHash(blockHash) {}

void BlockHashEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                             const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new BlockReqEvent(timestamp + interval, to, from, blockHash));
}

std::string BlockHashEvent::toString() const {
  return "BlockHashEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + u64ToHex(blockHash) + ")";
}
