#include <cmath>

#include "common/math.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxEvent::TxEvent(uint64_t timestamp, uint16_t from, uint16_t to, bool byHash, Tx tx)
    : Event(timestamp), from(from), to(to), byHash(byHash), tx(tx) {}

void TxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                      const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint32_t txId = tx >> 32;
  const std::unique_ptr<Node> &node = nodes[to];
  if (node->txPool.contains(txId))
    return;
  node->txPool.addTx(tx);
  std::vector<Peer *> peersWithoutTxs;
  for (auto &[_, peer] : node->peerMap)
    if (peer->id != from)
      peersWithoutTxs.push_back(peer);
  int sendTxNum = sqrt(peersWithoutTxs.size());
  for (int i = 0; i < sendTxNum; i++) {
    Peer *peer = peersWithoutTxs[i];
    const std::unique_ptr<Node> &peerNode = nodes[i];
    if (peerNode->txPool.contains(txId))
      continue;
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    uint64_t comingTimestamp = timestamp + interval;
    if (peerNode->minTxTimestamp.count(txId) && peerNode->minTxTimestamp[txId] <= comingTimestamp)
      continue;
    queue.push(new TxEvent(comingTimestamp, to, peer->id, false, tx));
    peerNode->minTxTimestamp[txId] = comingTimestamp;
  }
  for (int i = sendTxNum; i < peersWithoutTxs.size(); i++) {
    Peer *peer = peersWithoutTxs[i];
    const std::unique_ptr<Node> &peerNode = nodes[i];
    if (peerNode->txPool.contains(txId))
      continue;
    uint64_t interval = (global.minDelay + rand() % (global.maxDelay - global.minDelay)) * 3;
    uint64_t comingTimestamp = timestamp + interval;
    if (peerNode->minTxTimestamp.count(txId) && peerNode->minTxTimestamp[txId] <= comingTimestamp)
      continue;
    queue.push(new TxEvent(comingTimestamp, to, peer->id, true, tx));
    peerNode->minTxTimestamp[txId] = comingTimestamp;
  }
}

std::string TxEvent::toString() const {
  return "TxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + (byHash ? ", byHash: true, tx: " : ", byHash: false, tx: ") + hashHex(hashTx(tx)) +
         ")";
}
