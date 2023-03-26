#include "event/req_tx_event.h"
#include "emu/config.h"
#include "event/recv_req_tx_event.h"

ReqTxEvent::ReqTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void ReqTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                         const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new RecvReqTxEvent(timestamp + interval, from, to, txHash));
}

std::string ReqTxEvent::toString() const {
  return "ReqTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + u64ToHex(txHash) + ")";
}
