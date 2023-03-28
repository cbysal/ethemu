#include <cmath>

#include "emu/config.h"
#include "event/tx_event.h"
#include "event/tx_hash_event.h"

TxEvent::TxEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Transaction> &tx)
    : Event(timestamp), from(from), to(to), tx(tx) {}

void TxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                      const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  uint64_t hash = tx->hash();
  node->txPool[hash] = tx;
  std::vector<Peer *> peersWithoutTxs;
  for (auto &[_, peer] : node->peerMap)
    if (!peer->knownTransaction(hash))
      peersWithoutTxs.push_back(peer);
  for (int i = 0; i < std::sqrt(peersWithoutTxs.size()); i++) {
    Peer *peer = peersWithoutTxs[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new TxEvent(timestamp + interval, to, peer->id, tx));
    peer->markTransaction(hash);
  }
  for (int i = std::sqrt(peersWithoutTxs.size()); i < peersWithoutTxs.size(); i++) {
    Peer *peer = peersWithoutTxs[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new TxHashEvent(timestamp + interval, to, peer->id, hash));
    peer->markTransaction(hash);
  }
}

std::string TxEvent::toString() const {
  return "TxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + u64ToHex(tx->hash()) + ")";
}
