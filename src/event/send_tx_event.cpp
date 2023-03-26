#include "event/send_tx_event.h"
#include "emu/config.h"

SendTxEvent::SendTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, Transaction *tx) : Event(timestamp) {
  this->from = from;
  this->to = to;
  this->tx = tx;
}

void SendTxEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                          const std::vector<uint64_t> &nodeList, const std::unordered_map<uint64_t, Node *> &nodeMap) {
  uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
  queue.push(new RecvTxEvent(timestamp + interval, from, to, tx));
}

std::string SendTxEvent::toString() const {
  return "SendTxEvent (timestamp: " + std::to_string(timestamp) + ", from: " + idToString(from) +
         ", to: " + idToString(to) + ", tx: " + u64ToHex(tx->hash()) + ")";
}
