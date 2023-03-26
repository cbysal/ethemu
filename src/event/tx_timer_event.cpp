#include "event/tx_timer_event.h"

TxTimerEvent::TxTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void TxTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                           const std::vector<Node *> &nodes) {
  uint64_t interval = global.minTxInterval + rand() % (global.maxTxInterval - global.minTxInterval);
  queue.push(new TxTimerEvent(timestamp + interval));
  uint64_t from = rand() % nodes.size();
  Node *node = nodes[from];
  uint64_t to = node->peerMap[node->peerList[rand() % node->peerList.size()]->addr]->addr;
  Transaction *tx = new Transaction(from, to, node->nextNonce());
  queue.push(new RecvTxEvent(timestamp, from, from, tx));
}

std::string TxTimerEvent::toString() const { return "TxTimerEvent (timestamp: " + std::to_string(timestamp) + ")"; }
