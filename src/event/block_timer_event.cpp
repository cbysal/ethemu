#include "event/block_timer_event.h"
#include "event/block_event.h"

BlockTimerEvent::BlockTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void BlockTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                              const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[rand() % nodes.size()];
  std::shared_ptr<Block> parentBlock = node->blocksByNumber[node->current];
  std::vector<Tx> txs = node->getTxs();
  std::shared_ptr<Block> block = std::make_shared<Block>(parentBlock->hash(), node->id, parentBlock->number() + 1, txs);
  queue.push(new BlockEvent(timestamp, node->id, node->id, block));
  queue.push(new BlockTimerEvent(timestamp + 15000));
}

std::string BlockTimerEvent::toString() const {
  return "BlockTimerEvent (timestamp: " + std::to_string(timestamp) + ")";
}
