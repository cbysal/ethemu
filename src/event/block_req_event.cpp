#include "event/block_req_event.h"
#include "emu/config.h"
#include "event/block_event.h"

BlockReqEvent::BlockReqEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t blockHash)
    : Event(timestamp), from(from), to(to), blockHash(blockHash) {}

void BlockReqEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                            const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  std::shared_ptr<Block> block = readBlockByHash(db, node->addr, blockHash);
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new BlockEvent(timestamp + interval, to, from, block));
}

std::string BlockReqEvent::toString() const {
  return "BlockReqEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + u64ToHex(blockHash) + ")";
}
