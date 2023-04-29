#include <cmath>

#include "common/math.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxEvent::TxEvent(uint64_t timestamp, uint16_t from, uint16_t to, Tx tx)
    : timestamp(timestamp), from(from), to(to), tx(tx) {}

void TxEvent::process(const std::vector<Node *> &nodes) const {
  uint32_t txId = tx >> 32;
  Node *node = nodes[to];
  if (node->txPool->contains(txId))
    return;
  bool isAdded = node->txPool->addTx(tx);
  if (!isAdded) {
    node->resentTxs.emplace(from, to, tx);
    return;
  }
}

std::string TxEvent::toString() const {
  return "TxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + hashHex(hashTx(tx)) + ")";
}
