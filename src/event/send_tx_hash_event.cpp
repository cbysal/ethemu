#include "event/send_tx_hash_event.h"
#include "emu/config.h"
#include "event/recv_tx_hash_event.h"

SendTxHashEvent::SendTxHashEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t txHash)
    : Event(timestamp), from(from), to(to), txHash(txHash) {}

void SendTxHashEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                              const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new RecvTxHashEvent(timestamp + interval, from, to, txHash));
}

std::string SendTxHashEvent::toString() const {
  return "SendTxHashEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", txHash: " + u64ToHex(txHash) + ")";
}
