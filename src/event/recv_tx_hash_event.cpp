#include "event/recv_tx_hash_event.h"
#include "event/req_tx_event.h"

RecvTxHashEvent::RecvTxHashEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void RecvTxHashEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                              const std::vector<std::unique_ptr<Node>> &nodes) const {
  queue.push(new ReqTxEvent(timestamp, to, from, txHash));
}

std::string RecvTxHashEvent::toString() const {
  return "RecvTxHashEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + u64ToHex(txHash) + ")";
}
