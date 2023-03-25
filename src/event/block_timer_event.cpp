#include "event/block_timer_event.h"

BlockTimerEvent::BlockTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void BlockTimerEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
             const std::unordered_map<Address, Node *, AddrHash> &nodeMap) {
  Node *node = nodeMap.at(nodeList[rand() % nodeList.size()]);
  Block *parentBlock = readBlock(db, node->id, node->current);
  std::vector<Transaction *> txs;
  for (auto [hash, tx] : node->txPool) {
    txs.push_back(tx);
    if (txs.size() >= 200)
      break;
  }
  Block *block = new Block(parentBlock->hash(), node->addr, parentBlock->number + 1, txs);
  queue.push(new RecvBlockEvent(timestamp, node->addr, node->addr, block));
  queue.push(new BlockTimerEvent(timestamp + 15000));
}

std::string BlockTimerEvent::toString() const { return "BlockTimerEvent (timestamp: " + std::to_string(timestamp) + ")"; }