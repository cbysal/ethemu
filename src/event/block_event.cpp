#include <iostream>

#include "common/math.h"
#include "emu/config.h"
#include "event/block_event.h"
#include "event/tx_event.h"

BlockEvent::BlockEvent(uint64_t timestamp, Id from, Id to, bool byHash, Block *block)
    : Event(timestamp), from(from), to(to), byHash(byHash), block(block) {}

void BlockEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                         const std::vector<Node *> &nodes) const {
  Node *node = nodes[to];
  if (from == to) {
    std::vector<Tx> txs = node->txPool->pollTxs();
    block->setTxs(txs);
    std::cout << "New Block (Number: " << block->number << ", Hash: " << hashHex(block->hash())
              << ", Coinbase: " << idToString(node->id) << ", Txs: " << block->txs.size() << ")" << std::endl;
    std::cout << timestamp << ": events " << queue.size() << " id " << idToString(node->id) << " txpool "
              << node->txPool->size() << std::endl;
  }
  Hash hash = block->hash();
  if (block->number <= node->current)
    return;
  node->insertBlock(block);
  std::vector<Peer *> peersWithoutBlock;
  for (auto &[_, peer] : node->peerMap)
    if (!peer->knownBlock(hash))
      peersWithoutBlock.push_back(peer);
  for (int i = 0; i < std::sqrt(peersWithoutBlock.size()); i++) {
    Peer *peer = peersWithoutBlock[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new BlockEvent(timestamp + interval, to, peer->id, false, block));
    peer->markBlock(hash);
  }
  for (int i = std::sqrt(peersWithoutBlock.size()); i < peersWithoutBlock.size(); i++) {
    Peer *peer = peersWithoutBlock[i];
    uint64_t interval = (global.minDelay + rand() % (global.maxDelay - global.minDelay)) * 5 + rand() % 500;
    queue.push(new BlockEvent(timestamp + interval, to, peer->id, true, block));
    peer->markBlock(hash);
  }
  while (!node->resentTxs.empty()) {
    const auto &[from, to, byHash, tx] = node->resentTxs.front();
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
      auto it = peerNode->minTxTimestamp.find(txId);
      if (it != peerNode->minTxTimestamp.end() && it->second <= comingTimestamp)
        continue;
      queue.push(new TxEvent(comingTimestamp, to, peer->id, byHash, tx));
      peerNode->minTxTimestamp[txId] = comingTimestamp;
    }
    node->resentTxs.pop();
  }
}

std::string BlockEvent::toString() const {
  return "BlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + (byHash ? ", byHash: true, block: " : ", byHash: false, block: ") +
         hashHex(block->hash()) + ")";
}
