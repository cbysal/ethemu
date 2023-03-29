#include "event/tx_req_event.h"
#include "common/math.h"
#include "emu/config.h"
#include "event/tx_event.h"

TxReqEvent::TxReqEvent(uint64_t timestamp, Id from, Id to, Hash txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void TxReqEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                         const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  node->peerMap[from]->markTransaction(txHash);
  if (!node->txPool.contains(txHash))
    return;
  Tx tx = node->txPool.get(txHash);
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new TxEvent(timestamp + interval, to, from, tx));
}

std::string TxReqEvent::toString() const {
  return "TxReqEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + hashHex(txHash) + ")";
}
