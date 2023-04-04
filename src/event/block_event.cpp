#include <iostream>

#include "common/math.h"
#include "emu/config.h"
#include "event/block_event.h"

BlockEvent::BlockEvent(uint64_t timestamp, Id from, Id to, bool byHash, const std::shared_ptr<Block> &block)
    : Event(timestamp), from(from), to(to), byHash(byHash), block(block) {}

void BlockEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                         const std::vector<Node *> &nodes) const {
  if (from == to) {
    Node *node = nodes[block->coinbase()];
    std::cout << "New Block (Number: " << block->number() << ", Hash: " << hashHex(block->hash())
              << ", Coinbase: " << idToString(block->coinbase()) << ", Txs: " << block->txs.size() << ")" << std::endl;
    std::cout << "Txs in " << idToString(block->coinbase()) << ": " << node->txPool->size() << std::endl;
  }
  Node *node = nodes[to];
  Hash hash = block->hash();
  if (node->blocksByHash.count(hash))
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
}

std::string BlockEvent::toString() const {
  return "BlockEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + (byHash ? ", byHash: true, block: " : ", byHash: false, block: ") +
         hashHex(block->hash()) + ")";
}
