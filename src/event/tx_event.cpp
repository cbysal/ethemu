#include <cmath>

#include "common/math.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxEvent::TxEvent(uint64_t timestamp, uint16_t from, uint16_t to, bool byHash, Tx tx)
    : Event(timestamp), from(from), to(to), byHash(byHash), tx(tx) {}

void TxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                      const std::vector<Node *> &nodes) const {
  uint32_t txId = tx >> 32;
  Node *node = nodes[to];
  if (node->txPool->contains(txId))
    return;
  node->txPool->addTx(tx);
  node->minTxTimestamp.erase(txId);
  int sendTxNum = sqrt(node->peerList.size() - (from != to));
  int sentTxNum = 0;
  for (auto &[_, peer] : node->peerMap) {
    if (from == peer->id)
      continue;
    Node *peerNode = nodes[peer->id];
    if (peerNode->txPool->contains(txId))
      continue;
    bool byHash = (sentTxNum++) >= sendTxNum;
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    if (byHash)
      interval *= 3;
    uint64_t comingTimestamp = timestamp + interval;
    if (peerNode->minTxTimestamp.count(txId) && peerNode->minTxTimestamp[txId] <= comingTimestamp)
      continue;
    queue.push(new TxEvent(comingTimestamp, to, peer->id, byHash, tx));
    peerNode->minTxTimestamp[txId] = comingTimestamp;
  }
}

std::string TxEvent::toString() const {
  return "TxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + (byHash ? ", byHash: true, tx: " : ", byHash: false, tx: ") + hashHex(hashTx(tx)) +
         ")";
}
