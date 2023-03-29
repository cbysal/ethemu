#include "event/tx_timer_event.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxTimerEvent::TxTimerEvent(uint64_t timestamp) : Event(timestamp) {}

void TxTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                           const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minTxInterval + rand() % (global.maxTxInterval - global.minTxInterval);
  queue.push(new TxTimerEvent(timestamp + interval));
  Id from = rand() % nodes.size();
  const std::unique_ptr<Node> &node = nodes[from];
  Id to = node->peerList[rand() % node->peerList.size()];
  Tx tx = newTx(from, to, node->nextNonce());
  queue.push(new TxEvent(timestamp, from, from, false, tx));
}

std::string TxTimerEvent::toString() const { return "TxTimerEvent (timestamp: " + std::to_string(timestamp) + ")"; }
