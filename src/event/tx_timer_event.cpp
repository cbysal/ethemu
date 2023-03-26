#include "event/tx_timer_event.h"

TxTimerEvent::TxTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void TxTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                           const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minTxInterval + rand() % (global.maxTxInterval - global.minTxInterval);
  queue.push(new TxTimerEvent(timestamp + interval));
  uint64_t from = rand() % nodes.size();
  const std::unique_ptr<Node> &node = nodes[from];
  uint64_t to = node->peerList[rand() % node->peerList.size()];
  std::shared_ptr<Transaction> tx = std::make_shared<Transaction>(from, to, node->nextNonce());
  queue.push(new TxEvent(timestamp, from, from, tx));
}

std::string TxTimerEvent::toString() const { return "TxTimerEvent (timestamp: " + std::to_string(timestamp) + ")"; }
