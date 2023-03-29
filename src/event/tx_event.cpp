#include <cmath>

#include "common/math.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxEvent::TxEvent(uint64_t timestamp, uint16_t from, uint16_t to, bool byHash, Tx tx)
    : Event(timestamp), from(from), to(to), byHash(byHash), tx(tx) {}

void TxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                      const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  Hash hash = hashTx(tx);
  if (from != to)
    node->peerMap[from]->markTransaction(hash);
  if (node->txPool.contains(hash))
    return;
  node->txPool.addTx(tx);
  std::vector<Peer *> peersWithoutTxs;
  for (auto &[_, peer] : node->peerMap)
    if (!peer->knownTransaction(hash))
      peersWithoutTxs.push_back(peer);
  int sendTxNum = sqrt(peersWithoutTxs.size());
  for (int i = 0; i < sendTxNum; i++) {
    Peer *peer = peersWithoutTxs[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new TxEvent(timestamp + interval, to, peer->id, false, tx));
    peer->markTransaction(hash);
  }
  for (int i = sendTxNum; i < peersWithoutTxs.size(); i++) {
    Peer *peer = peersWithoutTxs[i];
    uint64_t interval = (global.minDelay + rand() % (global.maxDelay - global.minDelay)) * 3;
    queue.push(new TxEvent(timestamp + interval, to, peer->id, true, hash));
    peer->markTransaction(hash);
  }
}

std::string TxEvent::toString() const {
  return "TxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + (byHash ? ", byHash: true, tx: " : ", byHash: false, tx: ") + hashHex(hashTx(tx)) +
         ")";
}
