#include "event/body_event.h"
#include "emu/config.h"
#include "event/block_event.h"
#include "event/block_hash_event.h"

BodyEvent::BodyEvent(uint64_t timestamp, uint16_t from, uint16_t to, uint64_t blockHash,
                     const std::vector<std::shared_ptr<Transaction>> &txs)
    : Event(timestamp), from(from), to(to), blockHash(blockHash), txs(txs) {}

void BodyEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                        const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  if (!node->fetchedHeaders.count(blockHash))
    return;
  auto header = node->fetchedHeaders[blockHash];
  node->fetchedHeaders.erase(blockHash);
  auto block = std::make_shared<Block>(header, txs);
  node->insertBlock(block);
  std::vector<Peer *> peersWithoutBlock;
  for (auto &[_, peer] : node->peerMap)
    if (!peer->knownBlock(blockHash))
      peersWithoutBlock.push_back(peer);
  for (int i = 0; i < std::sqrt(peersWithoutBlock.size()); i++) {
    Peer *peer = peersWithoutBlock[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new BlockEvent(timestamp + interval, to, peer->id, block));
    peer->markBlock(blockHash);
  }
  for (int i = std::sqrt(peersWithoutBlock.size()); i < peersWithoutBlock.size(); i++) {
    Peer *peer = peersWithoutBlock[i];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new BlockHashEvent(timestamp + interval, to, peer->id, blockHash));
    peer->markBlock(blockHash);
  }
}

std::string BodyEvent::toString() const {
  return "BodyEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", blockHash: " + u64ToHex(blockHash) + ")";
}
