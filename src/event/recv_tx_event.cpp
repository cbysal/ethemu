#include "event/recv_tx_event.h"

RecvTxEvent::RecvTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Transaction> &tx)
    : Event(timestamp), from(from), to(to), tx(tx) {}

void RecvTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                          const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->txPool[tx->hash()] = tx;
  for (auto &[_, peer] : node->peerMap) {
    if (!peer->knownTransaction(tx->hash())) {
      queue.push(new SendTxEvent(timestamp, to, peer->addr, tx));
      peer->markTransaction(tx->hash());
    }
  }
}

std::string RecvTxEvent::toString() const {
  return "RecvTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + u64ToHex(tx->hash()) + ")";
}
