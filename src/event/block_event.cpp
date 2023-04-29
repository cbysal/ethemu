#include <iostream>

#include "common/math.h"
#include "emu/config.h"
#include "event/block_event.h"

BlockEvent::BlockEvent(uint64_t timestamp, Id from, Id to, Block *block)
    : timestamp(timestamp), from(from), to(to), block(block) {}

void BlockEvent::process(const std::vector<Node *> &nodes) const {
  Node *node = nodes[to];
  if (from == to) {
    std::vector<Tx> txs = node->txPool->pollTxs();
    block->setTxs(txs);
    std::cout << "New Block (Number: " << block->number << ", Hash: " << hashHex(block->hash())
              << ", Coinbase: " << idToString(node->id) << ", Txs: " << block->txs.size() << ")" << std::endl;
    std::cout << timestamp << " id " << idToString(node->id) << " txpool " << node->txPool->size() << std::endl;
  }
  if (block->number <= node->current)
    return;
  node->insertBlock(block);
  while (!node->resentTxs.empty()) {
    const auto &[from, to, tx] = node->resentTxs.front();
    uint32_t txId = tx >> 32;
    Node *node = nodes[to];
    if (node->txPool->contains(txId)) {
      node->resentTxs.pop();
      continue;
    }
    bool isAdded = node->txPool->addTx(tx);
    if (!isAdded) {
      break;
    }
    node->resentTxs.pop();
  }
}

std::string BlockEvent::toString() const {
  return "BlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", block: " + hashHex(block->hash()) + ")";
}
