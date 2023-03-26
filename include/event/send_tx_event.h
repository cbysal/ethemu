#pragma once

#include "event/event.h"
#include "event/recv_tx_event.h"

class SendTxEvent : public Event {
public:
  uint64_t from;
  uint64_t to;
  Transaction *tx;

  SendTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, Transaction *tx);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<Node *> &nodes);
  std::string toString() const;
};
