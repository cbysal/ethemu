#include "event/tx_timer_event.h"

TxTimerEvent::TxTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void TxTimerEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<uint64_t> &nodeList,
                           const std::unordered_map<uint64_t, Node *> &nodeMap) {
  uint64_t interval = global.minTxInterval + rand() % (global.maxTxInterval - global.minTxInterval);
  queue.push(new TxTimerEvent(timestamp + interval));
  uint64_t from = nodeList[rand() % nodeList.size()];
  Node *node = nodeMap.at(from);
  uint64_t to = node->peers[rand() % node->peers.size()]->addr;
  Transaction *tx = new Transaction(from, to, node->nextNonce());
  queue.push(new RecvTxEvent(timestamp, from, from, tx));
}

std::string TxTimerEvent::toString() const {
  return "TxTimerEvent (timestamp: " + std::to_string(timestamp) + ")";
}
