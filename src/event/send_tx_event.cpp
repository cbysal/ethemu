#include "event/send_tx_event.h"
#include "emu/config.h"

SendTxEvent::SendTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Transaction> &tx)
    : Event(timestamp), from(from), to(to), tx(tx) {}

void SendTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                          const std::vector<std::unique_ptr<Node>> &nodes) const {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new RecvTxEvent(timestamp + interval, from, to, tx));
}

std::string SendTxEvent::toString() const {
  return "SendTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + u64ToHex(tx->hash()) + ")";
}
