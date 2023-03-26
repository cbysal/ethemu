#include "emu/config.h"
#include "event/tx_hash_event.h"
#include "event/tx_req_event.h"

TxHashEvent::TxHashEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void TxHashEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                              const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new TxReqEvent(timestamp + interval, to, from, txHash));
}

std::string TxHashEvent::toString() const {
  return "TxHashEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + u64ToHex(txHash) + ")";
}
