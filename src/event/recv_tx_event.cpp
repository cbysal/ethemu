#include "event/recv_tx_event.h"

RecvTxEvent::RecvTxEvent(uint64_t timestamp, Address from, Address to, Transaction *tx) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->tx = tx;
}

void RecvTxEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
                          const std::unordered_map<Address, Node *, AddrHash> &nodeMap) {
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
  return "RecvTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + from.toString() +
         ", to: " + to.toString() + ", tx: " + tx->hash().toString() + ")";
}