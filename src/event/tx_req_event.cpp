#include "event/tx_req_event.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxReqEvent::TxReqEvent(uint64_t timestamp, uint16_t from, uint16_t to, uint64_t txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void TxReqEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                         const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  std::shared_ptr<Transaction> tx = node->txPool[txHash];
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new TxEvent(timestamp + interval, to, from, tx));
}

std::string TxReqEvent::toString() const {
  return "TxReqEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + u64ToHex(txHash) + ")";
}
