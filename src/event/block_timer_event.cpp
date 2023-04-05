#include <iostream>

#include "common/math.h"
#include "emu/config.h"
#include "event/block_event.h"
#include "event/block_timer_event.h"

BlockTimerEvent::BlockTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void BlockTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                              const std::vector<Node *> &nodes) const {
  Node *node = nodes[rand() % nodes.size()];
  std::shared_ptr<Block> parentBlock = node->blocksByNumber[node->current];
  std::vector<Tx> txs = node->getTxs();
  std::shared_ptr<Block> block = std::make_shared<Block>(parentBlock->number + 1, txs);
  queue.push(new BlockEvent(timestamp, node->id, node->id, false, block));
  queue.push(new BlockTimerEvent(timestamp + global.period));
  std::cout << "New Block (Number: " << block->number << ", Hash: " << hashHex(block->hash())
            << ", Coinbase: " << idToString(node->id) << ", Txs: " << block->txs.size() << ")" << std::endl;
  std::cout << "Txs in " << idToString(node->id) << ": " << node->txPool->size() << std::endl;
}

std::string BlockTimerEvent::toString() const {
  return "BlockTimerEvent (timestamp: " + std::to_string(timestamp) + ")";
}
