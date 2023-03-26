#include "event/block_timer_event.h"

BlockTimerEvent::BlockTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void BlockTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                              const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[rand() % nodes.size()];
  std::unique_ptr<Block> parentBlock = readBlock(db, node->id, node->current);
  std::vector<std::shared_ptr<Transaction>> txs;
  for (auto &[hash, tx] : node->txPool) {
    txs.push_back(tx);
    if (txs.size() >= 200)
      break;
  }
  std::shared_ptr<Block> block =
      std::make_shared<Block>(parentBlock->hash(), node->addr, parentBlock->number() + 1, txs);
  queue.push(new BlockEvent(timestamp, node->addr, node->addr, block));
  queue.push(new BlockTimerEvent(timestamp + 15000));
}

std::string BlockTimerEvent::toString() const {
  return "BlockTimerEvent (timestamp: " + std::to_string(timestamp) + ")";
}
