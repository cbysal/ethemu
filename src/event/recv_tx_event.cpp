#include <cmath>

#include "event/recv_tx_event.h"
#include "event/send_tx_hash_event.h"

RecvTxEvent::RecvTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Transaction> &tx)
    : Event(timestamp), from(from), to(to), tx(tx) {}

void RecvTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                          const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  uint64_t hash = tx->hash();
  node->txPool[hash] = tx;
  std::vector<Peer *> peersWithoutTxs;
  for (auto &[_, peer] : node->peerMap)
    if (!peer->knownTransaction(tx->hash()))
      peersWithoutTxs.push_back(peer);
  for (int i = 0; i < std::sqrt(peersWithoutTxs.size()); i++) {
    Peer *peer = peersWithoutTxs[i];
    queue.push(new SendTxEvent(timestamp, to, peer->addr, tx));
    peer->markTransaction(hash);
  }
  for (int i = std::sqrt(peersWithoutTxs.size()); i < peersWithoutTxs.size(); i++) {
    Peer *peer = peersWithoutTxs[i];
    queue.push(new SendTxHashEvent(timestamp, to, peer->addr, hash));
    peer->markTransaction(hash);
  }
}

std::string RecvTxEvent::toString() const {
  return "RecvTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + u64ToHex(tx->hash()) + ")";
}
