#include "event/recv_tx_event.h"

RecvTxEvent::RecvTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, Transaction *tx) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->tx = tx;
}

void RecvTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                          const std::vector<uint64_t> &nodeList, const std::unordered_map<uint64_t, Node *> &nodeMap) {
  Node *node = nodeMap.at(to);
  node->txPool[tx->hash()] = tx;
  for (auto peer : node->peerList) {
    if (!peer->knownTransaction(tx->hash())) {
      queue.push(new SendTxEvent(timestamp, to, peer->addr, tx));
      peer->markTransaction(tx->hash());
    }
  }
}

std::string RecvTxEvent::toString() const {
  return "RecvTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + tx->hash().toString() + ")";
}
