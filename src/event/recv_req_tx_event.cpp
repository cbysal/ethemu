#include "event/recv_req_tx_event.h"
#include "event/req_tx_event.h"

RecvReqTxEvent::RecvReqTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void RecvReqTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                             const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[to];
  std::shared_ptr<Transaction> tx = node->txPool[txHash];
  queue.push(new SendTxEvent(timestamp, to, from, tx));
}

std::string RecvReqTxEvent::toString() const {
  return "RecvReqTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + u64ToHex(txHash) + ")";
}
