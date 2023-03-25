#include "event/send_tx_event.h"

SendTxEvent::SendTxEvent(uint64_t timestamp, Address from, Address to, Transaction *tx) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->tx = tx;
}

void SendTxEvent::process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
             const std::unordered_map<Address, Node *, AddrHash> &nodeMap) {
  queue.push(new RecvTxEvent(timestamp, from, to, tx));
}

std::string SendTxEvent::toString() const {
  return "SendTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + from.toString() +
         ", to: " + to.toString() + ", tx: " + tx->hash().toString() + ")";
}