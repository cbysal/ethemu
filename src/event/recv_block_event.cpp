#include "event/recv_block_event.h"

RecvBlockEvent::RecvBlockEvent(uint64_t timestamp, Address from, Address to, Block *block) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->block = block;
}

void RecvBlockEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
                             const std::unordered_map<Address, Node *, AddrHash> &nodeMap) {
  Node *node = nodeMap.at(to);
  node->insertBlock(block);
  for (auto peer : node->peerList) {
    if (!peer->knownBlock(block->hash())) {
      queue.push(new SendBlockEvent(timestamp, to, peer->addr, block));
      peer->markBlock(block->hash());
    }
  }
}

std::string RecvBlockEvent::toString() const {
  return "RecvBlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + from.toString() +
         ", to: " + to.toString() + ", tx: " + block->hash().toString() + ")";
}