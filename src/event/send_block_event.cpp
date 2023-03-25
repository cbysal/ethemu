#include "event/send_block_event.h"

SendBlockEvent::SendBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, Block *block) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->block = block;
}

void SendBlockEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db,
                             const std::vector<uint64_t> &nodeList,
                             const std::unordered_map<uint64_t, Node *> &nodeMap) {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new RecvBlockEvent(timestamp + interval, from, to, block));
}

std::string SendBlockEvent::toString() const {
  return "SendBlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", block: " + block->hash().toString() + ")";
}
